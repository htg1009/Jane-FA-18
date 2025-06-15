//*******************************************************************************
//*  Langtext.h
//*  
//*  This file contains defines, types, prototypes, and globals 
//*  dealing with language and text actions.
//*******************************************************************************

//this should be the same as MAX_PLANES
#define LANG_MAX_PLANES		150

//*********************************************************************************************
//  Related defines for speech/sound catalogue
//*********************************************************************************************
#ifdef F15_SPEECH

#define SPCH_BETTY		0
#define SPCH_WSO		1000
#define SPCH_WNG1		2000
#define SPCH_WNG2		3000
#define SPCH_WNG3		4000
#define SPCH_WNG4		5000
#define SPCH_WNG5		6000
#define SPCH_WNG6		7000
#define SPCH_WNG7		8000
#define SPCH_WNG8		9000
#define SPCH_AWACS1		10000
#define SPCH_AWACS2		11000
#define SPCH_JSTARS1	12000
#define SPCH_TANKER1	13000
#define SPCH_TOWER1		14000
#define SPCH_TOWER2		15000
#define SPCH_US_GRND1	16000
#define SPCH_UK_GRND1	17000
#define SPCH_CAP1		18000
#define SPCH_CAP2		19000
#define SPCH_CAP3		20000
#define SPCH_SAR1		21000
#define SPCH_SAR2		22000
#define SPCH_SEAD1		23000
#define SPCH_SEAD2		24000
#define SPCH_US_BOMB1	25000
#define SPCH_UK_BOMB1	26000

#else

#define SPCH_WSO		1000
#define SPCH_SAR2		18000
#define SPCH_TANKER1	33000
#define SPCH_TOWER1		26000
#define SPCH_TOWER2		27000
#define SPCH_US_GRND1	32000
#define SPCH_UK_GRND1	32000
#define SPCH_CAP1		23000
#define SPCH_CAP2		24000
#define SPCH_CAP3		25000
#define SPCH_SEAD1		9000
#define SPCH_SEAD2		10000
#define SPCH_US_BOMB1	23000
#define SPCH_UK_BOMB1	24000


#define SPCH_BETTY		0
#define SPCH_WNG1		1000
#define SPCH_WNG2		2000
#define SPCH_WNG3		3000
#define SPCH_WNG4		4000
#define SPCH_WNG5		5000
#define SPCH_WNG6		6000
#define SPCH_WNG7		7000
#define SPCH_WNG8		8000
#define SPCH_NAVY1		9000
#define SPCH_NAVY2		10000
#define SPCH_NAVY3		11000
#define SPCH_NAVY4		12000
#define SPCH_NAVY5		13000
#define SPCH_LSO		14000
#define SPCH_AWACS1		15000
#define SPCH_AWACS2		16000
#define SPCH_JSTARS1	17000
#define SPCH_SAR1		18000
#define SPCH_FTR_USAF	19000
#define SPCH_FTR_NATO	20000
#define SPCH_FTR_RAF	21000
#define SPCH_FTR_RUS	22000
#define SPCH_ATK_USAF	23000
#define SPCH_ATK_RAF	24000
#define SPCH_ATK_RUS	25000
#define SPCH_TWR_NATO	26000
#define	SPCH_TWR_RUS	27000
#define SPCH_STRIKE		28000
#define SPCH_MARSHAL	29000
#define SPCH_FINAL		30000
#define SPCH_FAC_GRND	31000
#define SPCH_FAC_AIR	32000
#define SPCH_TANK_USN	33000
#define SPCH_TANK_USMC	34000

#endif

#define SPCH_BEARING	0
#define SPCH_O_CLOCK_H	52
#define SPCH_O_CLOCK_L  53
#define SPCH_O_CLOCK	54
#define SPCH_INT_O_CLOCK_H	60
#define SPCH_CARDINAL	20
#define SPCH_CARDINAL_INT	28
#define SPCH_CARDINAL2	36
#define SPCH_CARDINAL2_INT	44
#define SPCH_ALT		68
#define SPCH_WING_ACK	0
#define SPCH_INT_WING_ACK 10
#define SPCH_ZERO		0
#define SPCH_ZERO_INTENSE	10
#define SPCH_POINT		98
#define SPCH_ONE_MILE	99
#define SPCH_MILES		100
#define SPCH_COUNT_ONE	101
#define SPCH_STRIKE_CS	865
#define SPCH_THOUSAND	73
#define SPCH_ANGELS		74
#define SPCH_STATE		749
#define SPCH_BULLSEYE	598
#define SPCH_VERY_HIGH	68
#define SPCH_FIRST		201
#define SPCH_KAY		664

#define	WING_DISENGAGE	628						//  Wng1059  
#define WING_ENGAGED_OFFENSIVE	625			   	//  Wng1054 
#define WING_BOMBS_GONE	698						//  Wng1078 
#define WING_MAVERICK	696						//  Wng1076 
#define WING_MAGNUM 	577
#define WING_TWO		2						//  Wng1552 
#define WING_GROUP_BEARING	2287					//  Wng1044 
#define WING_SINGLE_BEARING	2289					//  Wng1046 
#define WING_BANDIT_BANDIT	402					//  Wng1372 
#define WING_UNKNOWN	400						//  Wng1370 
#define WING_RIGHT	58							//  Wng1251 
#define WING_LEFT	57							//  Wng1250 
#define WING_CLOSING	83						//  Wng1263 
#define WING_GOING_AWAY	84						//  Wng1265 
#define WING_FLANKING	82						//  Wng1264 
#define WING_GROUP	601							//  Wng1266 
#define WING_SINGLE	606							//  Wng1267 
#define WING_BANDIT	406							//  Wng1268 
#define WING_ADDITIONAL_GROUP_BEARING	2288			//  Wng1045 
#define WING_ADDITIONAL_SINGLE_BEARING	2290		//  Wng1047 

#define TANK_000_SPEED_300	13510			//	Tnk1012
#define TANK_ANGELS_10		13560			//	Tnk1048

#define TOWER_WINDS_ARE		435			//	Twr1004
#define TOWER_WINDS_000_AT	0			//	Twr1061
#define TOWER_WINDS_CALM	438			//	Twr1060
#define TOWER_SPEED_5		5			//	Twr1097
#define TOWER_CEILING_ANGELS_1	101			//	Twr1005
#define TOWER_VIS_HALF_MILE		100			//	Twr1041
#define TOWER_ANGELS_10		101			//	Twr1160

#define WSO_5_SEC_IMPACT	1317			//	Wso1119
#define WSO_60_SEC_IMPACT	1311			//	Wso1113
#define WSO_5_SEC_RELEASE	1298			//	Wso1100
#define WSO_60_SEC_RELEASE	1292			//	Wso1094
#define WSO_PULL_NOW		1352			//	Wso1202
#define WSO_5_SEC_PULL		1351			//	Wso1201
#define WSO_60_SEC_PULL		1345			//	Wso1195
#define WSO_120_MIN_NAV		1525			//	Wso1172
#define WSO_60_MILE_TARGET	1286			//	Wso1088
#define WSO_VOMIT			1757			//	Wso1653
#define WSO_4_BREATHS_SLOW	1750			//	Wso1646
#define WSO_4_BREATHS_FAST	1753			//	Wso1649
#define WSO_6_BREATHS_STRAIN	1755			//	Wso1651
#define WSO_CARDINAL_INTENSE	1448			//	Wso1606
#define WSO_ENEMY_CHOPPER	1201			//	Wso1003
#define WSO_BANDIT			1200			//	Wso1002

#define DEAD_SPACE	957

#define ADV_SPCH_START		600

//*********************************************************************************************
//*  End speech sound cat stuff
//*********************************************************************************************

//*********************************************************************************************
//*  Generic Message for Planes
//*********************************************************************************************
#define GM_DUCKS_AWAY		42







#define MAX_LSO_WATCH	20
#define	LWH_ON_CENTER	0x00000001
#define LWH_L_LEFT		0x00000002	
#define LWH_LEFT		0x00000004	
#define LWH_L_RIGHT		0x00000008	
#define LWH_RIGHT		0x00000010	
#define LWH_ON_GLIDE	0x00000020	
#define LWH_L_HIGH		0x00000040	
#define LWH_HIGH		0x00000080	
#define LWH_L_LOW		0x00000100	
#define LWH_LOW			0x00000200	
#define LWH_AOA_OK		0x00000400	
#define LWH_L_SLOW_AOA	0x00000800	
#define LWH_SLOW_AOA	0x00001000	
#define LWH_L_FAST_AOA	0x00002000	
#define LWH_FAST_AOA	0x00004000	
#define LWH_WAVEOFF		0x01000000	
#define LWH_WAVEOFF_BAD	0x02000000	

#define ASSET_RANGE		150

#define AI_ENEMY 0
#define AI_FRIENDLY 1
#define AI_NEUTRAL 2

#define TARGET_WEAPON 4
#define TARGET_LOCATION 5

#define AI_US	0
#define AI_UK	1
#define AI_NATO	2
#define AI_RUS	3
#define AI_ARAB 2
#define AI_FRENCH 3

#define MAXNUMRADIOMSGS		10		//  Maximum number of radio messages that can be stored.
#define MAXRADIOMSGSIZE		300     //   256     //  Maximum string length for a radio message.
									//  NOTE!!! GERMAN COULD NEED THIS LONGER.
#define MAXAISOUNDQUEUES	32		//  Maximum number of AI sound Queues

#define RADIOMSGDISPLAY		5000
#define AICOMMDISPLAY		7500

#define AI_VISUAL_RANGE_NM		10.0f
#define AI_ESCORT_RANGE_NM		1.5f

#define PAD_GLANCE	0x0001

#define MAX_DELAYED_DAMAGE	30

#define PING_WARNING_QUIET	20000

#define G_EFFECT_TIME	4000
#define G_RECOVER_TIME	45000
#define DOGFIGHT_SPEECH_DELAY 45000
#define GOOD_ENEMY_TIME	15000

//*******************************************************************************
//*  Bit Flags for lBombFlags
//*******************************************************************************
#define WSO_BOMB_IMPACT 0x01
#define WSO_BOMB_TREL	0x02
#define WSO_BOMB_TPULL	0x04
#define WSO_STEERING_MSG 0x08
#define WSO_NAV_MSGS	0x10
#define WSO_FENCE_IN	0x20
#define WSO_JSTARS_CHECK 0x40
#define WSO_DEEP_DOO_DOO 0x80
#define WSO_ENGINE1_BAD	 0x100
#define WSO_ENGINE2_BAD	 0x200
#define WSO_HYD_BAD		 0x400
#define WSO_TURN_BOMB_R	 0x800
#define WSO_TURN_BOMB_L	 0x1000
#define WSO_BOMB_LONGER	 0x2000
#define WSO_BOMB_SHORTER 0x4000
#define WSO_MASK_WARNING 0x8000
#define WSO_MASKING		 0x10000
#define WSO_LASER_ON	 0x20000
#define WSO_MUSIC_ON	 0x40000
#define WSO_G_RECOVER	 0x80000
#define WSO_NEG_G_RECOVER 0x100000
#define WSO_RADAR_OFF	 0x200000
#define WSO_JAMMER_ON	 0x400000
#define WSO_AWACS_CHECK	 0x800000
#define WSO_AWACS_TACTICAL	 0x1000000
#define WSO_FAC_CHECK	 0x2000000
#define WSO_MP_RELOAD_ALLOWED 0x8000000

//*******************************************************************************
//*  Bit Flags for iPlayerBombFlags
//*******************************************************************************
#define WSO_BOMB_HIT	0x01
#define WSO_SECONDAIRES	0x02
#define WSO_BULLET_HIT 0x00000004
#define WSO_NO_BOMB_MISS 0x08

//*******************************************************************************
//*  Bit Flags for lLaserStatusFlags
//*******************************************************************************
#define WSO_LASER_IS_ON			0x01
#define WSO_LASER_MASK_WARN		0x02
#define WSO_LASER_MASKED		0x04

//*******************************************************************************
//*  Bit Flags for iFACHitFlags
//*******************************************************************************
#define FAC_HIT_SHORT			0x01
#define FAC_HIT_LONG			0x02
#define FAC_HIT_LEFT			0x04
#define FAC_HIT_RIGHT			0x08
#define FAC_HIT_DAMAGE			0x10
#define FAC_HIT_DESTROYED		0x20
#define FAC_HIT_FRIEND			0x40
#define FAC_LASER				0x100
#define FAC_NO_LASER			0x200
#define FAC_LASER_ON			0x400
#define FAC_HAS_TARGET			0x10000

//*******************************************************************************
//*  Bit Flags for lAdvisorFlags
//*******************************************************************************
#define ADV_WAYPT_FRONT			0x00000001
#define ADV_WAYPT_BACK			0x00000002
#define ADV_WAYPT_LEFT			0x00000004
#define ADV_WAYPT_RIGHT			0x00000008
#define ADV_TARGET_FRONT		0x00000010
#define ADV_TARGET_BACK			0x00000020
#define ADV_TARGET_LEFT			0x00000040
#define ADV_TARGET_RIGHT		0x00000080
#define ADV_TARGET_UP			0x00000100
#define ADV_TARGET_BELOW		0x00000200
#define ADV_PITCH_UP			0x00000400  //  Not used
#define ADV_THROTTLE_UP			0x00000800
#define ADV_IN_RANGE			0x00001000
#define ADV_SHOOT				0x00002000
#define ADV_CARRIER_LAUNCH		0x00004000
#define ADV_GOOD_THROTTLE		0x00008000
#define ADV_TOO_CLOSE			0x00010000
#define ADV_TOO_FAR				0x00020000
#define ADV_MISSILE_FIRED		0x00040000
#define ADV_TAKEOFF_START		0x00080000
#define ADV_SPEED_ADVISOR		0x00100000
#define ADV_FIRST_PULL			0x00200000
#define ADV_CASUAL_BETTY		0x08000000
#define ADV_TAKEOFF				0x10000000
#define ADV_MISSILE				0x20000000
#define ADV_WAYPT				0x40000000
#define ADV_TARGET				0x80000000

//*************************************************************************************
//*  GENERIC BETTY DEFINES
//*************************************************************************************
#define		BETTY_WAY_DIST		1
#define		BETTY_CHAFF			2
#define		BETTY_FLARE			3
#define		BETTY_LEFT			4
#define		BETTY_RIGHT			5
#define		BETTY_MISSILE_OCLOCK	6
#define		BETTY_TARGET_DIST	7
#define		BETTY_PULL_UP		8
#define		BETTY_PULL_SECS		9
#define		BETTY_ZERO			10
#define		BETTY_TARGET_INTACT	11
#define		BETTY_TARGET_DESTROYED	12
#define		BETTY_TARGET_DAMAGED	13
#define		BETTY_ALERT			14
#define		BETTY_SAM_LAUNCH	15
#define		BETTY_ENEMY_U_D		16
#define		BETTY_ENEMY_POSITION		17
#define		BETTY_NET_HACK		18
#define		BETTY_MISSILE_LAUNCH	19
#define		BETTY_UP			20
#define		BETTY_DOWN			21
#define		BETTY_COUNT			22
#define		BETTY_LOW_ALT		23
#define		BETTY_SINK_RATE		24
#define		BETTY_FLIGHT_CONTROLS	25

//*******************************************************************************
//*  Bit Flags for lSREWarnFlags
//*******************************************************************************
#define WARN_SINK_RATE	0x1
#define WARN_FLIGHT_CONTROLS	0x2

//*******************************************************************************
//*  Bit Flags for lNoRepeatFlags
//*******************************************************************************
#define WE_HIT_SAID		0x00000001
#define WINCHESTER_SAID	0x00000002
#define	FORMING_UP_SAID	0x00000004
#define BOMB_MISS_SAID	0x00000008

//*******************************************************************************
//*  Bit Flags for lCarrierMsgFlags
//*******************************************************************************
#define MOTHER_SICK_MSG		0x00000001
#define BIRDS_AFIRM_MSG		0x00000002
#define	VAMPIRES_INBOUND_MSG	0x00000004
#define VAMPIRE_DIRECTION	0x000000F0
#define PLAYER_HIT_BULLET	0x00000100
#define PLAYER_HIT_MISSILE	0x00000200

typedef struct stLTMessageData {		//  Struture holds the data for one on screen message.
	int		iMessagePriority;		 	//  Priority of Message.
	long	lTimer;						//  Holds how many ticks left to display message.
	char	sMessageText[MAXRADIOMSGSIZE];	//  Holds the message string
} LTMessageData;

typedef struct stLTAICommMenuData {		//  Struture holds the data for one on screen message.
	long	lTimer;		  			//  Holds how many ticks left to display AI Comm Menu.
	void	(*AICommMenufunc)();	//  This is a pointer to the AI communications menu function to display the right menu.
	void	(*AICommKeyfunc)(int keyflag, WPARAM wParam);		//  This is a pointer to the AI communications menu function to interpret key strokes.
	int		lMaxStringPixelLen;		//  Holds the maximum pixel length of the longest menu line.
} LTAICommMenuData;

typedef struct stLTAISoundQueue {
	void	(*Soundfunc)(int planenum, int targetnum);
	int		planenum;
	int		targetnum;
	long	lTimer;
	int		priority;
} LTAISoundQueue;

typedef struct stDamageDelays {
	int		iPlaneNum;
	long	lDamageTimer;
	long	lDamagedSystem;
} DamageDelays;


//***********************************************************************************************************************************
BOOL LANGGetTransMessage(char *pszBuffer, DWORD maxbuffsize, DWORD msgid, DWORD languageid, ... );
int TXTWriteStringInBox(char *wstr, GrBuff *SREBuff, GrFontBuff *font, int MinX, int MinZ, int width, int height, int color, int newlinespace, long *xstart, int center, int spcbetweenlines = 0);
int TXTHeightOfStringInBox(char *wstr, GrFontBuff *font, int width, int height, int newlinespace);
int TXTGetMaxFontHeight(GrFontBuff *font);
int TXTGetMaxFontWidth(GrFontBuff *font);
int TXTGetMaxFontWidthNum(GrFontBuff *font);
void TXTDrawBoxFillABS(GrBuff *SREBuff, long MinX, long MinZ, long MaxX, long MaxZ, int bcolor);
void TXTDrawBoxFillRel(GrBuff *SREBuff, long MinX, long MinZ, long bwidth, long bheight, int bcolor);
long TXTGetStringPixelLength(char *wstr, GrFontBuff *font);
void AICDoMessagesAndComms();
void AICShowGroupCommOps();
int AICheckPartsOfGroup(PlaneParams *planepnt, int firstvalid, int lastvalid);
void AICShowFlightCommOps();
void AICShowDivisionElementCommOps();
void AICShowWingmanCommOps();
void AICShowGroundAttackCommOps();
void AICShowWeaponOptionCommOps();
void AICShowInterceptCommOps();
void AICShowTacticalCommOps();
void AICShowStatusCommOps();
void AICShowFormationCommOps();
int TXTGetFrontOfOptions(char *tempstr, int maxbuffersize, int optionnum);
void AICSetUpForCommKeys();
void AICRestoreFromCommKeys();
void AICCheckAIComm(int keyflag, WPARAM wParam);
void AICGroupCommKeyOps(int keyflag, WPARAM wParam);
void AICFlightCommKeyOps(int keyflag, WPARAM wParam);
void AICDivisionCommKeyOps(int keyflag, WPARAM wParam);
void AICElementCommKeyOps(int keyflag, WPARAM wParam);
void AICWingmanCommKeyOps(int keyflag, WPARAM wParam);
void AICFGroundAttackCommKeyOps(int keyflag, WPARAM wParam);
void AICDGroundAttackCommKeyOps(int keyflag, WPARAM wParam);
void AICEGroundAttackCommKeyOps(int keyflag, WPARAM wParam);
void AICWGroundAttackCommKeyOps(int keyflag, WPARAM wParam);
void AICGroundAttackCommKeyOps(int keyflag, WPARAM wParam, int sentto);
void AICStatusCommKeyOps(int keyflag, WPARAM wParam);
void AICFormationCommKeyOps(int keyflag, WPARAM wParam);
void AICDInterceptCommKeyOps(int keyflag, WPARAM wParam);
void AICEInterceptCommKeyOps(int keyflag, WPARAM wParam);
void AICWInterceptCommKeyOps(int keyflag, WPARAM wParam);
void AICInterceptCommKeyOps(int keyflag, WPARAM wParam, int sentto);
void AICTacticalCommKeyOps(int keyflag, WPARAM wParam);
int AICDisplayAnAICommMenuOption(GrBuff *usebuff, int messageid, int optionnum, int orgboxheight, int boxcolor, int textcolor, GrFontBuff *usefont, int startx, int starty, int width);
int AICGetMaxMenuPixelLen(int messageid, int optionnum, GrFontBuff *usefont);
int AICPixelLenGroupCommOps();
int AICPixelLenFlightCommOps();
int AICPixelLenDivisionElementCommOps();
int AICPixelLenWingmanCommOps();
int AICPixelLenGroundAttackCommOps();
int AICPixelLenWeaponOptionCommOps();
int AICPixelLenInterceptCommOps();
int AICPixelLenTacticalCommOps();
int AICPixelLenStatusCommOps();
int AICPixelLenFormationCommOps();
void AICWEngageBandits();
void AICEEngageBandits();
void AICDEngageBandits();
void AICFEngageBandits();
void AICEngageBandits(int firstvalid, int lastvalid);
void AICRadarToggle();
void AICRadarTogglePlane(int planenum, long *delaycnt);
//void AICAddSoundCall(LTAISoundQueue onesoundslot);
void AICAddSoundCall(void (*Soundfunc)(int planenum, int targetnum), int planenum, long delaycnt, int priority, int targetnum = -1);
void AICBasicAck(int planenum, int targetnum = -1);
void AICAddAIRadioMsgs(char *radiomsg, int priority);
void AICDisplayAIRadioMsgs();
void AICDisplayPausedRadioMsgs();
void AICDisengageMsg(int planenum, int targetnum = -1);
void AICGetCallSign(int planenum, char *callsign, int putlast = 1);
long AICGetCallSignNum(long callsignnum);
void AICEngageTogglePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid);
void AICNoJoyMsg(int planenum, int targetnum = -1);
void AICEngageMsg(int planenum, int targetnum = -1);
int AICGetIfPlaneGroupText(char *grouptext, PlaneParams *planepnt, int singletxt = 0);
int AICGetIfPlaneGroupRadarText(char *grouptext, PlaneParams *planepnt, int *spch_id, int *snd_id, int voice = -1, int ispopup = 0);
int AICGetTextHeadingToPlane(char *headingtxt, PlaneParams *planepnt, PlaneParams *target);
int AICGetTextHeadingGeneral(char *headingtxt, float heading);
void AICInitAIRadio();
void AICJammingToggle();
void AICJammingTogglePlane(int planenum, long *delaycnt);
void AICFlightStatus();
void AICFlightStatusPlane(int planenum, long *delaycnt);
void AICWedgeFormationChange();
void AICFormationChange(int formationid);
void AICBasicAckPlane(int planenum, long *delaycnt);
void AICFormationLoosen();
void AICFormationTighten();
void AICFormationModifyPlane(int planenum, long *delaycnt, float modifyby);
void AICWRejoinFlight();
void AICERejoinFlight();
void AICDRejoinFlight();
void AICFRejoinFlight();
void AICRejoinFlight(int firstvalid, int lastvalid);
void AICRejoinFlightPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid);
void AICWGrndAttackPrime();
void AICEGrndAttackPrime();
void AICDGrndAttackPrime();
void AICFGrndAttackPrime();
void AICWGrndAttackSecond();
void AICEGrndAttackSecond();
void AICDGrndAttackSecond();
void AICFGrndAttackSecond();
void AICWGrndAttackOpportunity();
void AICEGrndAttackOpportunity();
void AICDGrndAttackOpportunity();
void AICFGrndAttackOpportunity();
void AICWGrndAttackSEAD();
void AICEGrndAttackSEAD();
void AICDGrndAttackSEAD();
void AICFGrndAttackSEAD();
void AICGrndAttack(int firstvalid, int lastvalid, int targetpriority);
void AICGrndAttackPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int targetpriority);
void AICFWeaponOptionCommKeyOps(int keyflag, WPARAM wParam);
void AICDWeaponOptionCommKeyOps(int keyflag, WPARAM wParam);
void AICEWeaponOptionCommKeyOps(int keyflag, WPARAM wParam);
void AICWWeaponOptionCommKeyOps(int keyflag, WPARAM wParam);
void AICWeaponOptionCommKeyOps(int keyflag, WPARAM wParam, int sentto);
MBWayPoints	*AICGetBombWaypoint(int planenum, int targetpriority);
void AICGetBombAction(int planenum, MBWayPoints *bombway);
void AICInHotMsg(PlaneParams *planepnt);
void AICBombDropMsg(PlaneParams *planepnt);
void AICBomberBombDropMsg(PlaneParams *planepnt);
void AICBomberAttackMsg(PlaneParams *planepnt);
void AICMaverickLaunchMsg(PlaneParams *planepnt, int kind = 0);
//void AICEndGrndAttackMsg(PlaneParams *planepnt, float heading);
void AICEndGrndAttackMsg(int planenum, int heading);
void AIRWingmenRadioCall(long soundid, int planenum);
int AIRGetWingSoundRate(int planenum, int orgrate);
void AIRSendHeadingRadio(int headingval, int planenum);
int AICGetNextSndInt(char *tsptrorg, int *strcnt, int *done, int *spchvar = NULL);
void AIRBasicAckSndOld(int placeingroup);
void AIRBasicAckSnd(int planenum, int placeingroup);
long AIRWingNumSndID(int planenum, int placeingroup);
long AIRIntWingNumSndID(int planenum, int placeingroup);
void AIRDisengageSnd(int planenum, int placeingroup, int tempnum);
void AIREngageSnd(int planenum, int placeingroup, int headingval, int numingroup, int tempnum);
void AIRBombDropSnd(int planenum, int placeingroup, int tempnum);
void AIRMaverickLaunchSnd(int planenum, int placeingroup, int tempnum);
void AISimpleRadar(PlaneParams *planepnt);
void AIVectorIntercepts(PlaneParams *target, int side, float interceptnm = 120.0f);
int AIInAlertRange(PlaneParams *planepnt, float tdist);
int AIUpdateInterceptGroup(PlaneParams *planepnt, PlaneParams *target, int firstplane = 0);
void AISetInterceptTimer(PlaneParams *foundplane, int minutes);
void AIUpdateRadarThreat(PlaneParams *planepnt, PlaneParams *checkplane, int radarzone);
int AICheckIfAlreadyTarget(PlaneParams *searchplane, PlaneParams *planepnt, PlaneParams *checkplane);
int AICheckIfAnyTargets(PlaneParams *planepnt, PlaneParams *checklead, PlaneParams *searchplane, int *foundplane);
void AICRadarBanditCall(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range);
void AICVisualBanditCall(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range);
void AICAllReportContacts();
void AICWingReportContacts();
void AICReportContactsPlane(int planenum, long *delaycnt);
void AICMakeContactCall(int planenum, int targetnum = -1);
void AICCleanCall(int planenum, int targetnum = -1);
void AICReportContact(int planenum, int isaddition);
void AICReportContactPlane(int planenum, int isaddition, PlaneParams *orgtarget = NULL);
int AICPreviousTargetCall(PlaneParams *planepnt, int stopplace);
void AICGoingHotMsg(PlaneParams *planepnt);
void AICGoingColdMsgPN(int planenum, int targetnum = -1);
void AICGoingColdMsg(PlaneParams *planepnt);
//void AICTurnBackMsg(PlaneParams *planepnt);
int AIGetPlayerCoverType();
void AIDCover();
void AIECover();
void AIWCover();
void AIDOrbit();
void AIEOrbit();
void AIWOrbit();
void AISetUpOrbitPlayer(PlaneParams *orgplanepnt, int firstvalid, int lastvalid);
void AICSetBracket(int firstvalid, int lastvalid, int whichway);
void AICSetBracketPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int whichway);
void AICWBracketRight();
void AICEBracketRight();
void AICDBracketRight();
void AICWBracketLeft();
void AICEBracketLeft();
void AICDBracketLeft();
void AICWSplitHigh();
void AICESplitHigh();
void AICDSplitHigh();
void AICWSplitLow();
void AICESplitLow();
void AICDSplitLow();
void AICWDragRight();
void AICEDragRight();
void AICDDragRight();
void AICWDragLeft();
void AICEDragLeft();
void AICDDragLeft();
void AICCheckAIKeyUp(int keyflag, WPARAM wParam);
void AICShowOtherCommOps();
void AIC_AWACS_CommOps();
PlaneParams *AICheckTankerCloseBy(PlaneParams *planepnt, int findclosest = 0);
int AICPixelLenOtherCommOps();
int AICPixelLenAWACSCommOps();
void AICOtherCommKeyOps(int keyflag, WPARAM wParam);
void AIC_AWACS_CommKeyOps(int keyflag, WPARAM wParam);
void AICPlayerContactRefueler();
void AICPlayerContactRefuelerMaxTrap();
void AICPlayerContactRefueler4K();
void AICContactRefueler(PlaneParams *planepnt, int fuelrequested = 0);
void AICTankerContact1(int planenum, int targetnum = -1);
void AICTankerContact1Snd(int planenum, int tankernum, int tankamount);
void AICTankerContact2(int planenum, int targetnum = -1);
void AICTankerOn1(int planenum, int targetnum = -1);
void AICTankerOn2(int planenum, int targetnum = -1);
void AICReceivingFuel(int planenum, int targetnum = -1);
void AICTankerGoodHunting(int planenum, int targetnum = -1);
void AICTankerHaveDay(int planenum, int targetnum = -1);
void AICWSOThanksForGas(int planenum, int targetnum);
void AICWSOAcknowledged(int planenum, int targetnum);
void AICTankerDisconnecting(int planenum, int targetnum = -1);
void AICWSODisconnecting(int planenum, int targetnum = -1);
void AICEndTanking(PlaneParams *planepnt, long delayticks);
void AICTankerCheckSwitches(int planenum, int targetnum = -1);
void AICTankerHeading(int planenum, int targetnum = -1);
void AICTankerHeadingSpch(int planenum, int tankingnum, int msgid, long heading, long alt = 10);
void AICTankerBreakAway(int planenum, int targetnum = -1);
int AICheckHumanCover(PlaneParams *humanplane);
void AICSetUpEscortPlayer(PlaneParams *humanplane, int firstvalid, int lastvalid, int escortplane);
void AICSetUpEscortPlayerGroup(int planenum, long *delaycnt, int firstvalid, int lastvalid, int escortplane);
void AICW_RTB();
void AICE_RTB();
void AICD_RTB();
void AICF_RTB();
void AICReturnToBase(int firstvalid, int lastvalid);
void AICReturnToBasePlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int *firstreturn);
void AIC_RTB_Msg(int planenum, int targetnum = -1);
void AIC_Set_Takeoff_Msgs(int planenum, int targetnum = -1);
void AIC_Request_Takeoff_Msg(int planenum, int targetnum = -1);
void AIC_Clear_Takeoff_Msg(int planenum, int targetnum = -1);
long AICGetWindDirSpeed(int planenum, char *winddirstr, long *windspeednum);
long AICGetWindSpeed(int planenum, char *windspeedstr);
long AICGetCeiling(int planenum, char *ceilingstr);
long AICGetVisibility(int planenum, char *visiblestr);
void AICRDoClearTakeoff(int msgid, int towernum, int planenum, long towervoice, int winddirnum, int windspeednum, int ceilingnum, int visiblenum, int awacssndid);
void AIC_WSO_Run_em(int planenum, int targetnum = -1);
int AIGetClosestAWACS(PlaneParams *planepnt);
int AIGetClosestJSTARS(PlaneParams *planepnt);
int AIIsCAPing(PlaneParams *planepnt);
int AIIsSARCAPing(PlaneParams *planepnt);
int AICheckForEnemies(int planenum, int distnm);
void AICCheckForNewThreat(PlaneParams *planepnt);
void AIC_AWACS_Threat_Msg(int planenum, int targetnum);
void AIC_AWACS_Bogey_Dope_Msg(int planenum, int targetnum);
void AIC_AWACS_Picture_Msg(int planenum, int targetnum);
void AIC_AWACS_Threat_Takeoff_Msg(int planenum, int targetnum);
void AIC_AWACS_Calls(int calltype, PlaneParams *planepnt, PlaneParams *targplane);
int AICGetAspectText(char *aspectstr, float bearing, PlaneParams *planepnt);
void AICGetRangeText(char *rangestr, float rangenm);
void AIC_Scramble_Takeoff_Msg(int planenum, int targetnum);
void AICPlayerRequestLanding();
void AICRequestLanding(PlaneParams *planepnt);
void AIC_WSOLandingRequestMsg(int planenum, int targetnum = -1);
void AICTowerLandingReplyMsg(int planenum, int targetnum = -1);
void AICTowerAnglesHoldMsg(int planenum, int targetnum = -1);
void AICTowerAnglesHoldSnd(int towernum, int planenum, long towervoice, int holdalt);
void AICSetUpPlayerCAS(int planenum, MBWayPoints *bombway);
void AICheckInitialFACCalls(PlaneParams *planepnt);
void AIC_FAC_Contact_Msg(int planenum, int targetnum = -1);
void AIC_WSO_FAC_Hang_In(int planenum, int targetnum);
void AIC_FAC_Request_Msg(int planenum, int targetnum = -1);
void AIRFACHelpSnd(int msgid, int facnum, int planenum, int headingval, long groundvoice);
void AIC_FAC_NearMiss_Msg(int planenum, int targetnum = -1);
void AIC_FAC_Dead_Msg(int planenum, int targetnum = -1);
void AIC_FAC_GoodJob_Msg(int planenum, int targetnum = -1);
void AIC_FAC_EnemyGone_Msg(int planenum, int targetnum = -1);
float AICheckForGroundEnemies(FPoint centerpos, int distnm, int *founddistnm = NULL, int searchside = 0, int facnum = -1, float *mindist = NULL, int sameside = 0);
void AIReCheckingForGroundEnemies(FPoint centerpos, int distnm, int *founddistnm, int searchside, int validpos);
void AICDoAltPathMsgs(int planenum);
void AIC_AWACS_Task_Msg(int planenum, int targetnum);
void AIC_AWACS_Task2_Msg(int planenum, int targetnum);
void AIC_WSO_New_Path_Msg(int planenum, int targetnum);
int AICheckForFAC(FPoint *facloc = NULL);
void AIC_On_Station_Msg(int planenum, int targetnum = -1);
void AIC_Beginning_Human_Escort_Msg(int planenum, int targetnum);
void AICRequestPlayerSEAD();
void AICRequestSEAD(PlaneParams *planepnt);
void AICNoWeasels(int planenum, int awacsnum);
void AICWeaselsInBound(int planenum, int awacsnum);
void AICWeaselsOnWay(int planenum, int weaselnum);
void AICRequestPlayerCover();
void AICRequestCover(PlaneParams *planepnt);
void AICNoCover(int planenum, int awacsnum);
void AICCoverInBound(int planenum, int awacsnum);
void AICCoverOnWay(int planenum, int covernum);
void AIC_SAR_On_Way_Msg(int planenum, int targetnum = -1);
void AIC_SAR_Approach_Msg(int planenum, int targetnum = -1);
void AIC_SAR_Land_Msg(int planenum, int targetnum = -1);
void AIC_SAR_Success_Msg(int planenum, int targetnum = -1);
void AIC_SAR_Fail_Msg(int planenum, int targetnum = -1);
void AICCheckForHitMsgs(PlaneParams *planepnt, PlaneParams *shooter, int orgflightstat, WeaponParams *weaponpnt = NULL);
void AICDelayedPlayerDamageMsgs(int system_diffs, int weapon_index);
void AICCheckForDamageMsgs(PlaneParams *planepnt, int orgflightstat, int failed_system, WeaponParams *weaponpnt = NULL, AAAStreamVertex *AAA = NULL, DBWeaponType *pweapon_type = NULL);
void AIC_WSO_Kill_Msg(int planenum, int targetnum);
void AIC_Wingman_Kill_Msg(int planenum, int targetnum);
void AIRWingNumberKillSnd(int msgsnd, int planenum, int killnum);
void AIC_WSO_Wise_Ass_Msg(int planenum, int targetnum);
void AIC_WSO_Hit_Msg(int planenum, int targetnum);
void AIC_WSO_Missile_Hit_Msg(int planenum, int targetnum = -1);
void AIC_WSO_Bullet_Hit_Msg(int planenum, int targetnum = -1);
void AIC_WSO_AAA_Hit_Msg(int planenum, int targetnum = -1);
void AIC_WSO_Unknown_Hit_Msg(int planenum, int targetnum = -1);
void AIC_Wingman_Hit_Msg(int planenum, int targetnum);
void AIC_Wingman_Ejecting_Msg(int planenum, int targetnum = -1);
void AIC_WSO_Fox_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt = NULL);
void AIC_Fox_Missile_Msg(int planenum, int targetnum);
void AIC_WSO_Guns_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt = NULL);
int AICNoShotsAtPlayer(WeaponParams *ignoreweapon);
void AICDoWSOGunsBreakMsg(int planenum, int targetnum = -1);
void AIC_WSO_Bomb_Msgs(PlaneParams *planepnt, WeaponParams *weaponpnt = NULL);
void AIC_WSO_Release_Msgs(int planenum, int targetnum = -1);
void AIC_Get_Callsign_With_Number(int planenum, char *tempstr);
void AIC_Get_Flight_Callsign(int planenum, char *tempstr);
void AIC_WSO_Bomb_Speech(PlaneParams *planepnt, int bombphase, long timervar);
void AIC_WSO_Impact(PlaneParams *planepnt, long timervar);
void AIC_WSO_Impact_Msg(int planenum, int targetnum);
void AIC_WSO_Release(PlaneParams *planepnt, long timervar);
void AIC_WSO_Release_Msg(int planenum, int targetnum);
void AIC_WSO_TGT_Miles_Msg(int planenum, int targetnum);
void AIC_WSO_Pull(PlaneParams *planepnt, long timervar);
void AIC_WSO_Pull_Msg(int planenum, int targetnum);
void AIC_WSO_Check_Steering(PlaneParams *planepnt);
void AIC_WSO_Looking_Good_Msg(int planenum, int targetnum = -1);
void AIC_WSO_Watch_Steering_Msg(int planenum, int targetnum = -1);
void AIC_WSO_Nav_Speech(PlaneParams *planepnt);
void AIC_WSO_Nav_Minutes_Msg(int planenum, int targetnum);
int AICCheckForWingmanInVisualRange(PlaneParams *planepnt, PlaneParams *targetplane = NULL, int sametarget = 0);
void AICWingmanConfirmKill(int planenum, int targetnum);
void AIC_Eject_Wingman_Msg(int planenum, int targetnum);
void AIC_Wingman_Ejected_Msg(int planenum, int targetnum);
void AIC_Wingman_Ejected_Snd(int awacsnum, int planenum, int crashingnum);
void AICDoSAROnWayMsgs(PlaneParams *planepnt, int guyhurt = 0);
void AICDoNoSAROnWayMsgs(PlaneParams *planepnt, int guyhurt = 0);
void AICShortSARMsgs(PlaneParams *planepnt, int guyhurt = 0);
void AIC_Player_Read_Me(int planenum, int targetnum);
void AIC_Anyone_Read_Me(int planenum, int targetnum);
void AIC_Help_On_Way(int planenum, int targetnum);
void AIC_SAR_Inbound(int planenum, int targetnum);
void AIC_Enemy_Near_Hiding(int planenum, int targetnum);
void AIC_WSO_Hang_In(int planenum, int targetnum);
void AIC_Wingman_I_Understood(int planenum, int targetnum);
void AIC_Wingman_I_Ack(int planenum, int targetnum);
void AIC_WSO_Ack(int planenum, int targetnum);
void AIC_WSO_Grnd_Ack(int planenum, int targetnum);
void AIC_Wingman_Hurt(int planenum, int targetnum);
void AIC_Wingman_OK(int planenum, int targetnum);
int AIC_Enemy_Near(PlaneParams *planepnt);
void AICheckForMissileWarning(PlaneParams *planepnt, int weaponnum);
void AIC_WSO_MissileLaunch(int planenum, int targetnum);
void AIC_MissileLaunch(int planenum, int targetnum);
void AIGetPPositionStr(char *positionstr, int planenum, float heading, int highlow = 0);
void AIGetPositionStr(char *positionstr, int heading, int highlow = 0);
void AICFlightWeaponCheck();
void AICFlightWeaponCheckPlane(int planenum, long *delaycnt);
void AICWeaponCheck(int planenum, int targetnum = -1);
void AICWeaponCheckSnd(int planenum, int numradar, int numheater, int numgun, int numharm, int numagm, int numguided, int numdumb, int numcluster, int numpod);
int AIGetNumWeaponType(int planenum, int weapontype);
void AICheckTargetReaction(WeaponParams *weaponpnt, int justfired);
int AIMissileNoticed(WeaponParams *weaponpnt);
void AIDoCMDrop(PlaneParams *planepnt, WeaponParams *weaponpnt);
void AIDropCounterMeasure(PlaneParams *planepnt, WeaponParams *weaponpnt);
void AICDoMissileBreak(PlaneParams *planepnt, WeaponParams *weaponpnt);
void AISetBreakBehavior(PlaneParams *planepnt, float offangle, WeaponParams *weaponpnt);
void AIC_WSO_DoBreakMsg(int planenum, int targetnum);
void AIC_Wingman_Defensive_Msg(int planenum, int targetnum);
void AIC_WSO_DoCMMsg(int planenum, int targetnum);
void AIC_WSO_MissileInbound(int planenum, int targetnum);
void AICWSOEngageDefensive(int planenum, int targetnum);
void AIRAddContact(int planenum, int calltype, int placeingroup, int numingroup, int bearing360, int rangenm, int altval);
int AIRSendBearingRadio(int planenum, int bearing360, int placeingroup, int *sndids = NULL, long groundvoice = 0);
long AICRGetAIVoice(PlaneParams *planepnt);
int AICGetCountryFromPlaneID(int planeid);
int AIIsFACVoice(PlaneParams *planepnt);
int AIIsTankerVoice(PlaneParams *planepnt);
int AIIsSEADVoice(PlaneParams *planepnt);
int AIIsBomberVoice(PlaneParams *planepnt);
void AIRSendSentence(int planenum, int numids, int *sndids, long groundvoice = 0, int allowinaccel = 0, float volperc = 1.0);
int AIRGetChannel(int planenum);
long AIRGetCallSignID(int planenum);
long AIRGetIntenseCallSignID(int planenum);
long AIRGetCallSignSndID(long callsignnum, int intense);
long AIRGetWngCallNum(int planenum, int placeingroup);
long AIRGetIntWngCallNum(int planenum, int placeingroup);
long AIRGetHeadingSpeechID(int headingval, int planenum);
long AIRGetHeading2SpeechID(int headingval, int planenum);
void AIRRadarBanditCallSnd(PlaneParams *planepnt, PlaneParams *targplane, int bearing360, float range, int headingval, int numingroup);
long AICGetAspectSpeechID(int planenum, float bearing, PlaneParams *planepnt);
void AIRVisualBanditCallSnd(PlaneParams *planepnt, PlaneParams *targplane, float bearing, float range, int highlow);
//long AICGetRangeSpeechID(int planenum, float rangenm);
long AICGetRangeSpeechID(int planenum, float rangenm, int *sndids, int voice);
int AIGetPositionSpeechID(int planenum, float heading, int highlow, int *sndids, int voice, int intense = 0);
void AIRGenericSpeech(long messageid, int planenum, int targetnum = 0, float bearing = 0, int headingval = 0, int headingval2 = 0, float rangenm = 0, long alt = 0, long secs = 0, int numingroup = 0, long groundvoice = 0, int allowinaccel = 0, float volperc = 0.0f);
int AIRProcessSpeechVars(int *sndids, int genericid, int planenum, int targetnum = 0, float bearing = 0, int headingval = 0, int headingval2 = 0, float rangenm = 0, long alt = 0, long secs = 0, int numingroup = 0, long groundvoice = 0);
int AIRGetNumSpeech(int tempval);
long AIRGetFlightSndID(int planenum);
void AICSetUpTowerVoice(FPointDouble runwaypos);
long AIGetTowerCallsign(int planenum, long *retvoice = NULL);
long AIGetFACCallsign(int planenum, long *retvoice = NULL);
void AICCheckForFriendlyTraffic(int planenum, int targetnum);
void AICTankerDeadMsg(int planenum, int targetnum = -1);
void AICSARDeadMsg(int planenum, int targetnum = -1);
void AIC_SAR_Hit_Msg(int planenum, int targetnum = -1);
void AIC_SARMissileWarn(int planenum, int targetnum = -1);
void AICAWACSDeadMsg(int planenum, int targetnum = -1);
void AIC_AWACSMissileWarn(int planenum, int targetnum = -1);
void AIC_JSTARSAttackWarn(int planenum, int targetnum = -1);
void AICJSTARSDeadMsg(int planenum, int targetnum = -1);
void AICBomberDeadMsg(int planenum, int targetnum = -1);
void AICBomberHitMsg(int planenum, int targetnum = -1);
void AICSEADDeadMsg(int planenum, int targetnum = -1);
void AICCoverSweepMsg(int planenum, int targetnum);
void AICCheckForBanditCalls(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange);
void AICCAPRadarCall(PlaneParams *planepnt, PlaneParams *targplane, float targetbearing, float targetrange);
void AICCAPTallyCall(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange);
void AICBomberBanditCall(PlaneParams *planepnt, PlaneParams *targetpnt, float targetbearing, float targetrange);
void AICCheckForEngageMsg(int planenum, int targetnum);
void AICCAPEngagingMsg(int planenum, int targetnum);
void AICEscortInterceptMsg(int planenum, int targetnum);
void AIC_CAP_Kill_Msg(int planenum, int targetnum);
void AICCAPDeadMsg(int planenum, int targetnum);
void AIC_CAPMissileWarn(int planenum, int targetnum);
void AICCAPDone(int planenum, int targetnum = -1);
void AIC_CAP_WinchesterMsg(int planenum, int targetnum = -1);
void AICShowEscortReleaseCommOps();
int AICPixelLenEscortReleaseCommOps();
void AICEscortReleaseCommKeyOps(int keyflag, WPARAM wParam);
int AICDisplayEscorts(GrBuff *usebuff, int messageid, int optionnum, int orgboxheight, int boxcolor, int textcolor, GrFontBuff *usefont, int startx, int starty, int width, int planenum);
void AICPlayerCoverDone(int planenum, int targetnum);
void AICPlayerSEADCoverDone(int planenum, int targetnum);
int AICMorePlanesNear(PlaneParams *planepnt);
void AICRequestPlayerPicture();
void AICDelayPictureRequest(int planenum = 0, int tempnum = -1);
void AICContinuePicture(int nextdelay, int ftminrange);
int AICContinuePictureNearPlane(int *nextdelay, int ftminrange, PlaneParams *planepnt, int awacsnum, int currentcnt, int hasgroups, int additional);
int AICContinueBullsEyePicture(int *nextdelay, int ftminrange, PlaneParams *planepnt, int awacsnum, int currentcnt, int hasgroups, int additional);
int AIC_AWACS_Hide_Near_Planes(PlaneParams *planepnt, int awacsnum = 0);
void AICRequestPlayerBogieDope();
void AIC_AWACS_Unchanged(int planenum, int targetnum = -1);
void AIC_AWACS_Clean(int planenum, int awacsnum);
void AIC_AWACS_Pict_Clean(int planenum, int targetnum = -1);
void AIC_AWACS_SAR_OK(int planenum, int awacsnum);
void AICBombResultSpch(WeaponParams *W, int hit, int secondaries, int grndside = -1, int directhit = 0);
void AIC_WSO_Bomb_Hit(int planenum, int targetnum = -1);
void AIC_WSO_Secondaires(int planenum, int targetnum = -1);
void AIC_WSO_Bomb_Miss(int planenum, int targetnum = -1);
void AIC_AWACS_Acknowledged(int planenum, int awacsnum);
int AICheckForMoreAG(PlaneParams *planepnt, WeaponParams *ingnore);
int AICheckForMoreBullets(PlaneParams *planepnt, WeaponParams *ingnore);
void AIC_FormOn_Human_Msg(int planenum, int targetnum);
void AILookPlayerEscort(PlaneParams *planepnt);
void AIC_Spot_Human_Escort_Msg(int planenum, int targetnum);
void AIC_Release_Human_Escort_Msg(int planenum, int targetnum);
void AIC_Bombers_RTB_Msg(int planenum, int targetnum);
void AICMagnumLaunchMsg(PlaneParams *planepnt);
void AICMusicOnOffMsg(int planenum, int musicon);
void AICSEADAttackMsg(int planenum, int attacktype);
void AICSEADWinchesterMsg(int planenum, int targetnum);
void AIC_WSO_Check_Msgs(int planenum, int checktype);
void AIC_Human_Beginning_Escort_Msg(int planenum, int targetnum);
void AIC_WSO_Begin_Attack_Msgs(int planenum, int targetnum);
void AICPlayerJSTARSCheckIn();
void AICPlayerJSTARSCheckOut();
void AIC_WSO_JSTARS_Check_In(int planenum, int targetnum);
void AIC_JSTARS_Check_Reply(int planenum, int targetnum);
void AIC_WSO_JSTARS_Check_Out(int planenum, int targetnum);
void AIC_WSO_JSTARS_Request_New(int planenum, int targetnum);
void AIC_JSTARS_Acknowledge(int planenum, int targetnum);
void AIC_JSTARS_Give_Target_Loc();
int AIC_JSTARS_GetNextTarget();
void AIC_JSTARS_Give_Next_Target_Loc();
void AIC_JSTARS_Target_Loc(int jstarnum, int playernum);
void AIC_JSTARS_No_Targets(int planenum, int targetnum);
void AICWingCASAttackMsg(int planenum, int targetnum = -1);
void AICWingFinishCASAttackMsg(int planenum, int targetnum = -1);
void AICWingReadyToEngageMsg(int planenum, int targetnum = -1);
void AICWingReadyToAttackMsg(int planenum, int targetnum = -1);
void AICWingSpreadOutMsg(int planenum, int targetnum = -1);
void AICWingCloseUpMsg(int planenum, int targetnum = -1);
void AICWingOrbitHereMsg(int planenum, int targetnum = -1);
void AICWingCoverMsg(int planenum, int targetnum = -1);
void AIC_WSO_SpikeCall(PlaneParams *planepnt, void *radarsite, PlaneParams *pingplane = NULL, int sitetype = 3);
void AIC_Wingman_SpikeCall(PlaneParams *planepnt, void *radarsite, PlaneParams *pingplane = NULL, int sitetype = 3);
void AIC_SEAD_SpikeCall(PlaneParams *planepnt, void *radarsite, PlaneParams *pingplane = NULL, int sitetype = 3);
int AICCheckAllFences(float x, float z);
int AICCheckOneFence(int num_polys, MPoint vertexlist[], float x, float z);
void AICFenceCall(int fenceval, int planeid = -1);
void AIC_WSO_RadarCheck(int planenum, int targetnum = -1);
void AIC_WSO_WinderCheck(int planenum, int targetnum = -1);
void AIC_WSO_SparrowCheck(int planenum, int targetnum = -1);
void AIC_WSO_SlammerCheck(int planenum, int targetnum = -1);
void AIC_WSO_MudCheck(int planenum, int targetnum = -1);
void AIC_WSO_IFFCheck(int planenum, int targetnum = -1);
void AIC_WSO_EngineCheck(int planenum, int targetnum = -1);
void AIC_WSO_TEWSCheck(int planenum, int targetnum = -1);
void AIC_WSO_RadioCheck(int planenum, int targetnum = -1);
void AIC_WSO_LightCheck(int planenum, int targetnum = -1);
void AIC_WSO_PlatformCheck(int planenum, int targetnum = -1);
void AICWingmanAAAReport(int planenum, int targetnum);
void AICWSOAAAReport(int planenum, int targetnum);
void AICReportWingmanDamage(int planenum, int targetnum = -1);
void AIC_WSO_ReportDamage(int planenum, int targetnum);
void AIC_WSO_Eject(int planenum, int targetnum = -1);
void AIC_WSO_Trouble(int planenum, int targetnum = -1);
void AIC_WSO_CheckPanel(int planenum, int targetnum = -1);
void AIC_WSO_FuelLeak(int planenum, int targetnum = -1);
void AIC_WSO_On_Fire(int planenum, int targetnum = -1);
void AIC_WSO_MainHyd(int planenum, int targetnum = -1);
void AIC_WSO_CentComp(int planenum, int targetnum = -1);
void AIC_WSO_TargetPod(int planenum, int targetnum = -1);
void AIC_WSO_NavPod(int planenum, int targetnum = -1);
void AIC_WSO_AutoPilot(int planenum, int targetnum = -1);
void AIC_WSO_PACS(int planenum, int targetnum = -1);
void AIC_WSO_Pressure(int planenum, int targetnum = -1);
void AIC_WSO_Laser_On(int planenum, int targetnum = -1);
void AIC_WSO_Laser_Off(int planenum, int targetnum = -1);
void AIC_WSO_Loosing_Lase(int planenum, int targetnum = -1);
void AIC_WSO_Pod_Masked(int planenum, int targetnum = -1);
void AIC_WSO_Music_On(int planenum, int targetnum = -1);
void AIC_WSO_Music_Off(int planenum, int targetnum = -1);
void AICWAttackMyTarget();
void AICWHelpMe();
void *AIGroundTargetNear(FPoint centerpos, int distft, int *grndtype);
void AICFSortBandits();
void AIC_Basic_Ack(int planenum, int targetnum = -1);
void AIC_Basic_Neg(int planenum, int targetnum = -1);
void AIC_Attack_Target_Response(int planenum, int targetnum = -1);
void AIC_Help_Me_Response(int planenum, int targetnum = -1);
void AIC_Help_Me_Neg_Response(int planenum, int targetnum = -1);
void AICRequestClosestTanker();
void AIC_AWACS_CheckForTanker(int planenum, int targetnum = -1);
void AIC_AWACS_Tanker_Request_Speech(int planenum, int awacsnum, int tankernum, float bearing, float rangenm);
void AICDoSortDelay(int planenum, int targetnum = -1);
void AICDoSort(int planenum, int targetnum = -1);
void AICDoSortMsg(int planenum, int sortnum, int number = 1);
void AIRDoSortSpeech(int planenum, int sortnum, int number, int type);
int AICCheckVoiceNumInGroup(int planenum, int voice, int placeingroup);
float AIGetPlaneJammingMod(PlaneParams *planepnt, float radaranglepitch, float radarangleyaw, float widthangle, float heightangle);
void AIC_WSO_Report_Contacts_Delay(int planenum = -1, int targetnum = -1);
void AIC_Wing_Report_Contacts_Delay(int planenum = -1, int targetnum = -1);
void AIC_WSO_Report_Contacts(int planenum, int targetnum = -1);
int AIRCheckCallSignSpchExists(int sndid);
void AICDoEndBox(int idnum);
void AICGetEndTextTop(int idnum, char *endstr);
void AICGetEndTextBottom(int idnum, char *endstr);
void RestorePostExitView();
long AICGetFtSpeechID(int planenum, float rangenm);
void AICDoWSOgStuff();
void AICDoWSONoVariablesMsg(int planenum, int targetnum);
void AICDoWSOEnemyDisengaging(int planenum, int targetnum);
void AIC_WSO_BanditCall(int planenum, int targetnum);
int AITightenSpacingOK(int planenum, float modifyby);
int FormSpacingOK(int planenum, int skipnum, float modifyby);
void AIC_Wingman_BreakLRMsg(int planenum, int targetnum);
void AICAfterEjectionSoundCancel();
int AIIsCampaignJSTARS(PlaneParams *checkplane);
int AICAllowThisRadio(int planenum, int priority);
void AICNotifyShipInbound(int planenum, int targetnum);
void AICStrikeClear(int planenum, int targetnum = -1);
void AICStrikeContactMarshal(int planenum, int targetnum = -1);
void AICGetNumberText(char *numstr, float numval);
void AICMarshalGivePushTime(int planenum, int targetnum = -1);
void AICMarshalCheckSnd(int planenum, long visrange);
void AICMarshalGivePushTimeSnd(int planenum, float bearing, long range, float angles, int minutes, int minutes_now);
long AICGetCarrierWindDirSpeed(int planenum, char *winddirstr, long *windspeednum);
long AICGetCarrierWindSpeed(int planenum, char *windspeedstr);
long AICGetCarrierCeiling(int planenum, char *ceilingstr);
long AICGetCarrierVisibility(int planenum, char *visiblestr);
void AICEstablishedMarshal(int planenum, int targetnum = -1);
void AICCommencingPush(int planenum, int targetnum = -1);
void AICPlatform(int planenum, int targetnum = -1);
void AICLandingDistance(int planenum, int targetnum = 10);
void AIC_ACLSContact(int planenum, int targetnum = -1);
void AIC_ACLSDisplay(int planenum, int targetnum = -1);
void AIC_ACLSConcur(int planenum, int targetnum = -1);
void AICCallBall(int planenum, int targetnum = -1);
void AICSeeBall(int planenum, int targetnum = -1);
void AICRogerBall(int planenum, int targetnum = -1);
void AICCheckPlayerCarrierLanding();
void AICReturnToMarshall(int planenum, int targetnum = -1);
void AICCommenceNow(int planenum, int targetnum = -1);
void AICJustMessage(int msgnum, int msgsnd, int voicenum= 28000);
void AICCheckLSOForPlane(PlaneParams *planepnt, int firsttime = 0);
void AICGetRGBColors(int bcolor, int *redval, int *greenval, int *bluecolor);
void AICWWeaponOptionUnguidedAll();
void AICEWeaponOptionUnguidedAll();
void AICDWeaponOptionUnguidedAll();
void AICFWeaponOptionUnguidedAll();
void AICWWeaponOptionUnguidedHalf();
void AICEWeaponOptionUnguidedHalf();
void AICDWeaponOptionUnguidedHalf();
void AICFWeaponOptionUnguidedHalf();
void AICWWeaponOptionUnguidedSingle();
void AICEWeaponOptionUnguidedSingle();
void AICDWeaponOptionUnguidedSingle();
void AICFWeaponOptionUnguidedSingle();
void AICWWeaponOptionGuidedAll();
void AICEWeaponOptionGuidedAll();
void AICDWeaponOptionGuidedAll();
void AICFWeaponOptionGuidedAll();
void AICWWeaponOptionGuidedHalf();
void AICEWeaponOptionGuidedHalf();
void AICDWeaponOptionGuidedHalf();
void AICFWeaponOptionGuidedHalf();
void AICWWeaponOptionGuidedSingle();
void AICEWeaponOptionGuidedSingle();
void AICDWeaponOptionGuidedSingle();
void AICFWeaponOptionGuidedSingle();
void AICWWeaponOptionMissileAll();
void AICEWeaponOptionMissileAll();
void AICDWeaponOptionMissileAll();
void AICFWeaponOptionMissileAll();
void AICWWeaponOptionMissileHalf();
void AICEWeaponOptionMissileHalf();
void AICDWeaponOptionMissileHalf();
void AICFWeaponOptionMissileHalf();
void AICWWeaponOptionMissileSingle();
void AICEWeaponOptionMissileSingle();
void AICDWeaponOptionMissileSingle();
void AICFWeaponOptionMissileSingle();
void AICWWeaponOptionDefault();
void AICEWeaponOptionDefault();
void AICDWeaponOptionDefault();
void AICFWeaponOptionDefault();
void AICWeaponOptionSet(int firstvalid, int lastvalid, int weaponoption);
void AICWeaponOptionSetPlane(int planenum, long *delaycnt, int firstvalid, int lastvalid, int weaponoption);
int AICPlaneHasWeaponOption(int planenum);
void AICShowFlightReportCommOps();
void AICFlightReportCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenFlightReportCommOps();
void AICShowContactReportCommOps();
void AICContactReportCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenContactReportCommOps();
void AICShowThreatCallCommOps();
void AICThreatCallCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenThreatCallCommOps();
void AICShowEngagementCommOps();
void AICEngagementCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenEngagementCommOps();
void AICShowAlphaCheckCommOps();
void AICAlphaCheckCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenAlphaCheckCommOps();
void AICShowJSTARSCommOps();
void AICJSTARSCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenJSTARSCommOps();
void AICShowTankerCommOps();
void AICTankerCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenTankerCommOps();
void AICShowTowerCommOps();
void AICTowerCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenTowerCommOps();
void AICShowFACCommOps();
void AICFACCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenFACCommOps();
void AICShowEscortCommOps();
void AICEscortCommKeyOps(int keyflag, WPARAM wParam);
int AICPixelLenEscortCommOps();
int AIGetHumanEscortPlane(int planenum);

int AIRIsFighter(int planenum);
int AIRIsAttack(int planenum);
int AICGetEmitterType(int radarid, int isgun, int isinfo, char *emitterstr);
int AICGetBanditID(int banditidnum, char *idstr, int ischick);
void AIC_TellNumInPicture(int planenum, int numgroups, int hasgroups);
void AIC_TellNumInPictureSnd(int planenum, int numgroups, int hasgroups);
void AICBroadcastBanditCall(int planenum, int foundplane);
void AIC_AWACSGoTactical(int var1 = -1, int var2 = -1);
void AIC_AWACSGoBullseye(int var1 = -1, int var2 = -1);
void AIC_AWACSGoDigitalBullseye(int var1 = -1, int var2 = -1);
void AICContinueWingPicture(int planenum, int nextdelay);
void AICFlightFuelCheck(int planenum = 0, int targetnum = 0);
void AICFlightFuelCheckPlane(int planenum, long *delaycnt);
void AICFuelCheck(int planenum, int targetnum = 0);
void AICAlphaCheck(int planenum, int targetnum);
void AICAlphaCheckSnd(int planenum, int targetnum, float bearing, float range, float altitude, int awacsnum, int voice);
float AICGetBullseyeBearing(int planenum, float *range);
float AICGetIPBearing(int planenum, float *range);
float AICGetTargetBearing(int planenum, float *range);
float AICGetHomeBearing(int planenum, float *range);
float AICGetTankerBearing(int planenum, float *range, float *altitude);
float AICGetDivertBearing(int planenum, float *range);
float AICGetPackageBearing(int planenum, float *range, float *altitude);
void AICPlayerRequestTakeOff();
void AICFlightPositionCheck(int planenum = 0, int targetnum = 0);
void AICFlightPositionCheckPlane(int planenum = 0, long *delaycnt = 0, int leadplane = 0);
void AICPositionCheck(int planenum = 0, int leadplanenum = 0);
void AICPlayerRequestSAR();
void AICShowEscortEngageCommOps();
int AICPixelLenEscortEngageCommOps();
void AICEscortEngageCommKeyOps(int keyflag, WPARAM wParam);
void AICShowEscortSEADCommOps();
int AICPixelLenEscortSEADCommOps();
void AICEscortSEADCommKeyOps(int keyflag, WPARAM wParam);
void AICShowEscortCASCommOps();
int AICPixelLenEscortCASCommOps();
void AICEscortCASCommKeyOps(int keyflag, WPARAM wParam);
void AICShowEscortRejoinCommOps();
int AICPixelLenEscortRejoinCommOps();
void AICEscortRejoinCommKeyOps(int keyflag, WPARAM wParam);
int AIRejoinableEscorts(PlaneParams *planepnt, PlaneParams *leadprotect);
int AICCheckFACClose(PlaneParams *planepnt);
void AICCheckInWithFAC(int planenum, int targetnum = 0);
void AICCheckInWithFACSnd(int planenum, int faccallsign, int numagm, int numguided, int numdumb, int numcluster, int numpod);
int AICGetClosestFAC(PlaneParams *planepnt);
void AIC_FACAck(int planenum, int targetnum = 0);
void AIC_Established(int planenum, int targetnum = 0);
void AIC_FAC_RequestTarget(int planenum, int targetnum = 0);
void AIC_FAC_RequestNewTarget(int planenum, int targetnum = 0);
void AIC_FAC_Blind(int planenum, int targetnum = 0);
void AIC_FAC_9Line(int planenum, int targetnum = 0);
void AIC_FAC_9LineSnd(int planenum, int facnum, int closestWP, float targbearing, float targdist, int targetelev, int targetsnd, int smokeid, float smokebearing, float smokerange, float friendlybearing, int friendrangenm, int threatsnd, float threatbearing, float threatrange);
void AIC_FAC_4Line(int planenum, int facnum = 0);
void AIC_FAC_4LineSnd(int planenum, int facnum, float targbearing, float targdist, int targetsnd, int threatsnd, float threatbearing, float threatrange);
void AIC_GenericFACCall(int planenum, int targetnum = 0);
void AICReportFACTarget(int facnum, int planenum = 0, int reporttype = 0, int usecopy = 0);
void AICPutSmokeOnFACTarget(int facnum);
void AIC_FAC_Miss(int planenum, int targetnum = -1);
void AIC_FAC_MissSnd(int planenum, int facnum, int voice, int msgsnd, int distft);
void AICGetFACWeather(int facnum, char *tempstr, int *sndids, int *numids);
int AICGetTargetType(void *pTarget, int iTargetType, char *tempstr);
int AICGetThreatType(void *pTarget, int iTargetType, char *tempstr);
void AICGetClosestThreat(FPointDouble position, int iSide, float maxrange, void **pTarget, int *iTargetType);
int AICGetClosestWP(FPointDouble position, int planenum);
void AIC_GenericMsgPlane(int planenum, int targetnum);
void AICDoCommSort(PlaneParams *planepnt);
void AIC_GenericMsgPlaneBearing(int planenum, int targetnum, float bearing);
void AICDoCommTargetSpot();
void AICDoCommStrobeSpot();
void AIC_BolterStart(int planenum, int targetnum = -1);
void AIC_Ask_Declare(int orgplanenum, int targetnum = 0);
void AIC_AWACS_Declare(int planenum, int targetnum);
void AIC_AWACS_CheckInOut();
void AICCheckAdvisor();
void AICCheckTakeOffAdvisor();
void AICCheckPlayerTakeOff();
void AICDoAdvisorMsg(int msgnum, int targetnum = -1);
void AIC_ACLS_Switch(PlaneParams *planepnt, int on = 1);
void AICRequestEmergencyApproach(int planenum, int targetnum = -1);
int AICGetTowerVoice(int planenum);
void AICHoldMarshalStack(int planenum);
int AICDeckClear(PlaneParams *planepnt);
void AICPlayerSeesBall(int planenum, int targetnum = -1);
void AICPlayerClara(int planenum, int targetnum = -1);
void AICPlayerRequestTaxi(int planenum, int targetnum = -1);
void AICTowerInbound(int planenum, int targetnum = -1);
void AICGenericBettySpeech(int msgtype, int bearing = 0, int rangenm = 0, int cnt = 0, int other1 = 0, int other2 = 0);
void AICStrikeSwitchControl(int planenum, int targetnum = -1);
void AICGiveHackMessage(int planenum);
void AICDelayedSoundEffect(int soundeffect, int volume);
void AICInitExternalVars();


// in aiflight.cpp
int GetNumRocketsPerPod(int weaponid);


void AICNextTextColor();
void AICNextBoxColor();
void ControlJumpPlanes(void);
void RestoreFromJump();
void DoJumpGame(void);
void SetUpForPlayerJump();
void debug_do_next_plane();
void debug_do_closest_plane();

#if 0
void SRESndTest();
void SRESndTest2();
void SRESndTest2A(int planenum, int targetnum);
void SRESndTest2B(int planenum, int targetnum);
#endif

#ifdef SRELANGTEXTSTUFF
LTMessageData		gRadioMsgs[MAXNUMRADIOMSGS];
LTAICommMenuData	gAICommMenu;
LTAISoundQueue		gAISoundQueue[MAXAISOUNDQUEUES];
int					iNumRadioDisplay = 3;
int					iNumRadioHistDisplay = 10;
int					iRadioDisplayFlags = 2;
long				lRadioDisplayDur = 5000;
int					iMsgBoxColor = 199;  //  was 10;
int					iMsgTextColor = 207;  // was 1
int					iNoSelectTextColor = 201;
int					iAIShowRadarRange = 0;
char				cAIGoalSame[NUMGOALS];
long				lEventMinuteTimer = 0;
long				lEventMinutesPassed = 0;
int					iAIEventDelay[NUMEVENTS];					
int					iAIChangedEventFlags[30];
long				lAreaEventTimer = 0;
long				lAreaEventOccuranceFlags = 0;
long				l2AreaEventOccuranceFlags = 0;
int					iAIShiftDown = 0;
long				lDebugPlaneSRE;
long				lDebugPlaceSRE;
int					iAIHumanLanding = 0;
long				lAIHumanLandingTimer = -1;
long				lAIHumanAWACSTimer = -1;
int					iAIHumanTankerFlags = 0;
int					iFACState;
int					iFACTargetPos;
long				lFACTimer;
FPoint				fpFACPosition;
long				lBombTimer = 0;
long				lBombFlags = 0;
long				lWSOSpeakTimer = -1;
int					iUseBullseye = 1;
long				lAWACSPictTimer = -1;
int					iPlayerBombFlags = 0;
int					iCurrEyePoint = -1;
long				lJSTARSFlags = 0;
long				lJSTARSDead = 0;
int					iJSTARSTarget = 0;
WeaponType			*pChaffType = NULL;
WeaponType			*pFlareType = NULL;
WeaponType			*pEjectioSeatType = NULL;
WeaponType			*pFriendlyChuteType = NULL;
WeaponType			*pEnemyChuteType = NULL;
WeaponType			*pGuyOnGroundType = NULL;
WeaponType			*pLandingGateType = NULL;
WeaponParams		*pLandingGates[MAX_GATES];
ObjectHandler		*pRocket5 = NULL;
ObjectHandler		*pRocket275 = NULL;
long				lPlayerSARTimer = -1;
long				lPlayerDownedTimer = -1;
long				lFenceCheckTimer = -1;
int					iFenceCnt = 0;
int					iFenceIn = 0;
int					iInJump = 0;
long				lJumpTimer = 0;
char				*pJumpStr = NULL;
long				lNoticeTimer = -1;
long				lPlayerGroupTimer = -1;
char				cAAAFiring = 0;
char				cCurrentLook = 0;
DamageDelays		gDamageEvents[MAX_DELAYED_DAMAGE];
char				cPlayerArmor[8][40];
char				cPlayerDamage[8][40];
long				lNoBulletHitMsgs = -1;
long				lNoOtherBulletHitMsgs = -1;
long				lNewDamageFlags;
long				lNoCheckPlanelMsgs = -1;
long				lLaserStatusFlags = 0;
long				lLastAverageMovePlane = -1;
int					iAIChaffTotal[7];
int					iAIFlareTotal[7];
int					iAI_ROE[3];
int					iAISortNumber = 0;
WeaponParams		*pCurrentGBU;
int					iDoAllAAA = 0;
double				dLastPlayerV = 0;
int					iPlayerRunwayID = -1;
long				lPlayerRunwayVoice = -1;
long				lFenceFlags = 0;
WeaponParams		*pPlayerChute = NULL;
long				lEasyBomberTimer = -1;
int					iEasyBombVar = 0;
long				lEasyAATimer = -1;
int					iEasyAAVar = 0;
int					iEndGameState = 0;
int					iMaxEnemyGrndSkill = 0;
long				lLeftOrgTimeToFire = -1;
long				lLeftTimeToFire = -1;
long				lRightOrgTimeToFire = -1;
long				lRightTimeToFire = -1;
long				lLOrgAMADTimeToFire = -1;
long				lLAMADTimeToFire = -1;
long				lROrgAMADTimeToFire = -1;
long				lRAMADTimeToFire = -1;
long				lLeftTimeToExplode = 90000;
long				lRightTimeToExplode = 90000;
long				lLAMADTimeToExplode = 90000;
long				lRAMADTimeToExplode = 90000;
long				lFireFlags = 0;
int					WingNumDTWSTargets = 0;
int					WingDTWSTargets[8];
long				lWSOgTimer = -1;
long				lDogSpeechTimer = -1;
long				lDefensiveTimer = -1;
FPoint				fpPlayerGroupFormation[8];
long				lNetSpeechTimer = -1;
long				lPlayerBulletCount= 0;
char				cPlayerVulnerable = 1;
char				cPlayerLimitWeapons = 1;
int					iAAAFireAlreadyChecked = 0;
long				lAfterJump = 0;
long				lVisualOverloadTimer = -1;
CameraInstance		PreViewActionCamera;
long				lViewTimer = -1;
int					PreViewActionCockpitSeat, PreViewActionCurrentCockpit;
long				lForceEndTimer = -1;
double				dGlideSlope = 3.5f;
int					iCarrierWatch = -1;
int					iSpeechMute = 0;
char				cAWACSNewCnt = 0;
double				dFACRange = 0;
void				*pFACTarget = NULL;
int					iFACTargetType = -1;
float				fFACRange = -1.0f;
int					iFACHitFlags = 0;
long				lAdvisorFlags = 0;
long				lAdvisorTimer = 0;
ANGLE				AdvisorLastPitch = 0;
float				fAdvisorLastThrottle = 0;
long				lLSOWatchHist[MAX_LSO_WATCH];
long				lLSORating = 0;
int					iLSOWatchHistIndex = 0;
int					iFirstTimeInFrame = 1;
int					iMaxPlanesOnDeck = 27;
float				fAim9SunOffPitch = 0;
float				fAim9SunOffYaw = 0;
long				lAOAWarnTimer = -1;
long				lSREWarnFlags = 0;
long				lSREWarnTimer = -1;
DWORD				dwSpeedBrakeSnd = 0;
long				lNoRepeatTimer[LANG_MAX_PLANES];
long				lNoRepeatFlags[LANG_MAX_PLANES];
long				lCarrierHitMsgTimer = -1;
long				lCarrierBirdsAfirmTimer = -1;
long				lCarrierVampireTimer = -1;
long				lCarrierMsgFlags = 0;
int					iWingmanRadio = 1;
int					iATCRadio = 1;
int					iOtherRadio = 1;
int					iOtherRadioRangeNM = 20;
int					iHUDR = 0;
int					iHUDG = 0;
int					iHUDB = 0;
int					iCommR = 0;
int					iCommG = 0;
int					iCommB = 0;
int					iInactiveCommR = 0;
int					iInactiveCommG = 0;
int					iInactiveCommB = 0;
int					iCommBoxR = 0;
int					iCommBoxG = 0;
int					iCommBoxB = 0;
int					iEndBoxOpt = 0;
int					iEscOpt = 0;
int					iKeyboardOpt = 0;
int					iReturnACMView = 0;
int					iMShowFriend = 0;
float				fMeatBallOffsetFT = 100.0f;
float				fFCWarningAdjust = 0;
float				fFCWarningAdjustRoll = 0;
int					iAllowMultiPause = 0;
#else
extern LTMessageData	gRadioMsgs[];
extern LTAICommMenuData	gAICommMenu;
extern LTAISoundQueue	gAISoundQueue[];
extern int				iNumRadioDisplay;
extern int				iNumRadioHistDisplay;
extern int				iRadioDisplayFlags;
extern long				lRadioDisplayDur;
extern int				iMsgBoxColor;
extern int				iMsgTextColor;
extern int				iNoSelectTextColor;
extern int				iAIShowRadarRange;
extern char				cAIGoalSame[NUMGOALS];
extern long				lEventMinuteTimer;
extern long				lEventMinuteTimer;
extern long				lEventMinutesPassed;
extern int				iAIEventDelay[NUMEVENTS];					
extern int				iAIChangedEventFlags[30];
extern long				lAreaEventTimer;
extern long				lAreaEventOccuranceFlags;
extern long				l2AreaEventOccuranceFlags;
extern int				iAIShiftDown;
extern long				lDebugPlaneSRE;
extern long				lDebugPlaceSRE;
extern int				iAIHumanLanding;
extern long				lAIHumanLandingTimer;
extern long				lAIHumanAWACSTimer;
extern int				iAIHumanTankerFlags;
extern int				iFACState;
extern int				iFACTargetPos;
extern long				lFACTimer;
extern FPoint 			fpFACPosition;
extern long				lBombTimer;
extern long				lBombFlags;
extern long				lWSOSpeakTimer;
extern int				iUseBullseye;
extern long				lAWACSPictTimer;
extern int				iPlayerBombFlags;
extern int				iCurrEyePoint;
extern long				lJSTARSFlags;
extern long				lJSTARSDead;
extern int				iJSTARSTarget;
extern WeaponType		*pChaffType;
extern WeaponType		*pFlareType;
extern WeaponType		*pEjectioSeatType;
extern WeaponType		*pFriendlyChuteType;
extern WeaponType		*pEnemyChuteType;
extern WeaponType		*pGuyOnGroundType;
extern WeaponType		*pLandingGateType;
extern WeaponParams		*pLandingGates[MAX_GATES];
extern ObjectHandler 	*pRocket5;
extern ObjectHandler	*pRocket275;
extern long				lPlayerSARTimer;
extern long				lPlayerDownedTimer;
extern long				lFenceCheckTimer;
extern int				iFenceCnt;
extern int				iFenceIn;
extern int				iInJump;
extern long				lJumpTimer;
extern char				*pJumpStr;
extern long				lNoticeTimer;
extern long				lPlayerGroupTimer;
extern char				cAAAFiring;
extern char				cCurrentLook;
extern DamageDelays		gDamageEvents[];
extern char				cPlayerArmor[8][40];
extern char				cPlayerDamage[8][40];
extern long				lNoBulletHitMsgs;
extern long				lNoOtherBulletHitMsgs;
extern long				lNewDamageFlags;
extern long				lNoCheckPlanelMsgs;
extern long				lLaserStatusFlags;
extern long				lLastAverageMovePlane;
extern int				iAIChaffTotal[];
extern int				iAIFlareTotal[];
extern int				iAI_ROE[];
extern int				iAISortNumber;
extern WeaponParams		*pCurrentGBU;
extern int				iDoAllAAA;
extern double			dLastPlayerV;
extern int				iPlayerRunwayID;
extern long				lPlayerRunwayVoice;
extern long				lFenceFlags;
extern WeaponParams		*pPlayerChute;
extern long				lEasyBomberTimer;
extern int				iEasyBombVar;
extern long				lEasyAATimer;
extern int				iEasyAAVar;
extern int				iEndGameState;
extern int				iMaxEnemyGrndSkill;
extern long				lLeftOrgTimeToFire;
extern long				lLeftTimeToFire;
extern long				lRightOrgTimeToFire;
extern long				lRightTimeToFire;
extern long				lLOrgAMADTimeToFire;
extern long				lLAMADTimeToFire;
extern long				lROrgAMADTimeToFire;
extern long				lRAMADTimeToFire;
extern long				lLeftTimeToExplode;
extern long				lRightTimeToExplode;
extern long				lLAMADTimeToExplode;
extern long				lRAMADTimeToExplode;
extern long				lFireFlags;
extern int				WingNumDTWSTargets;
extern int				WingDTWSTargets[8];
extern long				lWSOgTimer;
extern long				lDogSpeechTimer;
extern long				lDefensiveTimer;
extern FPoint			fpPlayerGroupFormation[8];
extern long				lNetSpeechTimer;
extern long				lPlayerBulletCount;
extern char				cPlayerVulnerable;
extern char				cPlayerLimitWeapons;
extern int				iAAAFireAlreadyChecked;
extern long				lAfterJump;
extern long				lVisualOverloadTimer;
extern CameraInstance	PreViewActionCamera;
extern long				lViewTimer;
extern int				PreViewActionCockpitSeat, PreViewActionCurrentCockpit;
extern long				lForceEndTimer;
extern double			dGlideSlope;
extern int				iCarrierWatch;
extern int				iSpeechMute;
extern char				cAWACSNewCnt;
extern double			dFACRange;
extern void				*pFACTarget;
extern int				iFACTargetType;
extern float			fFACRange;
extern int				iFACHitFlags;
extern long				lAdvisorFlags;
extern long				lAdvisorTimer;
extern ANGLE			AdvisorLastPitch;
extern float			fAdvisorLastThrottle;
extern long				lLSOWatchHist[MAX_LSO_WATCH];
extern long				lLSORating;
extern int				iLSOWatchHistIndex;
extern int				iFirstTimeInFrame;
extern int				iMaxPlanesOnDeck;
extern float			fAim9SunOffPitch;
extern float			fAim9SunOffYaw;
extern long				lAOAWarnTimer;
extern long				lSREWarnFlags;
extern long				lSREWarnTimer;
extern DWORD			dwSpeedBrakeSnd;
extern long				lNoRepeatTimer[LANG_MAX_PLANES];
extern long				lNoRepeatFlags[LANG_MAX_PLANES];
extern long				lCarrierHitMsgTimer;
extern long				lCarrierBirdsAfirmTimer;
extern long				lCarrierVampireTimer;
extern long				lCarrierMsgFlags;
extern int				iWingmanRadio;
extern int				iATCRadio;
extern int				iOtherRadio;
extern int				iOtherRadioRangeNM;
extern int				iHUDR;
extern int				iHUDG;
extern int				iHUDB;
extern int				iCommR;
extern int				iCommG;
extern int				iCommB;
extern int				iInactiveCommR;
extern int				iInactiveCommG;
extern int				iInactiveCommB;
extern int				iCommBoxR;
extern int				iCommBoxG;
extern int				iCommBoxB;
extern int				iEndBoxOpt;
extern int				iEscOpt;
extern int				iKeyboardOpt;
extern int				iReturnACMView;
extern int				iMShowFriend;
extern float 			fMeatBallOffsetFT;
extern float			fFCWarningAdjust;
extern float			fFCWarningAdjustRoll;
extern int				iAllowMultiPause;
#endif

#define AIR_ZERO           2000
#define AIR_ONE            2001 
#define AIR_TWO            2002
#define AIR_THREE          2003
#define AIR_FOUR           2004
#define AIR_FIVE           2005
#define AIR_SIX            2006
#define AIR_SEVEN          2007
#define AIR_EIGHT          2008
#define AIR_Nine           2009
#define AIR_DISENGAGE_MSG  2010
#define AIR_NO_JOY_MSG     2011
#define AIR_ENGAGE_MSG     2012
#define AIR_NORTH          2013
#define AIR_EAST           2014
#define AIR_SOUTH          2015
#define AIR_WEST           2016
#define AIR_NORTH_EAST     2017
#define AIR_SOUTH_EAST     2018
#define AIR_SOUTH_WEST     2019
#define AIR_NORTH_WEST     2020
#define AIR_GROUP          2021
#define AIR_BANDIT         2022
#define AIR_IN_HOT         2023
#define AIR_BOMB_DROP      2024
#define AIR_MAVERICK_LAUNCH 2025
#define AIR_END_GRND_ATTACK 2026 
#define AIR_BANDITS 2027
#define AIR_STINGRAY 2028 
#define AIR_WC_TWO			2102
#define AIR_WC_THREE		2103
#define AIR_WC_FOUR			2104
#define AIR_WC_FIVE			2105
#define AIR_WC_SIX			2106
#define AIR_WC_SEVEN		2107
#define AIR_WC_EIGHT		2108
#define AIR_SR_TWO			2112
#define AIR_SR_THREE		2113
#define AIR_SR_FOUR			2114
#define AIR_SR_FIVE			2115
#define AIR_SR_SIX			2116
#define AIR_SR_SEVEN		2117
#define AIR_SR_EIGHT		2118
#define AIR_ALT_HIGH		2120
#define AIR_ALT_MED			2121
#define AIR_ALT_LOW			2122
#define AIR_ALT_WEEDS		2123
#define AIR_ADD_GROUP_BR	2130
#define AIR_SINGLE_BR		2131
#define AIR_GROUP_BR		2132
#define AIR_ADD_SINGLE_BR	2133
#define AIR_CLEAN			2134
#define AIR_ONE_MILE		2201

