
#ifndef __RADAR_H
#define __RADAR_H



typedef struct _RadarLadderType
{
	FPoint	RawRadarPoint[6];
	RotPoint3D RadarPoints[6];

} RadarLadderType;

typedef struct{
	GrBuff *Buff;
	int XOfs,YOfs;
	int XOfs2,YOfs2;
	int Width,Height;
	int AnchX, AnchY;

	TextureBuff *pTexture;		// Texture

}RDRSymType;

#define NUM_RDR_SYMBOLS		79

#define	RDR_CIRCLE	         0
#define RDR_ATN_ELV          1
#define RDR_ATN_AZ	         2
#define RDR_ACQ	             3
#define RDR_CLOSE_STAR       4
#define RDR_OPEN_BOX         5
#define RDR_CLOSE_BOX        6
#define RDR_OPEN_STAR        7
#define	RDR_UP_ARROW         8
#define	RDR_DOWN_ARROW       9
#define	RDR_LEFT_ARROW       10
#define	RDR_RIGHT_ARROW      11
#define TWS_AIRCRAFT_LOCK    12
#define TWS_AIRCRAFT         13
#define TWS_GROUND           14
#define TWS_MISSLE           15
#define TWS_MISSLE_LOCK      16
#define WYPT_CIRCLE          17
#define WYPT_HOME            18
#define TARGET_SYMB          19
#define START_BOMBING_PNT    20
#define HRM_BOX1             21
#define HRM_BOX2             22
#define HRM_BOX3             23
#define SMALL_WYPT           24
#define SMALL_HOME           25
#define SMALL_TARGET         26
#define SMALL_START_BOMB_PNT 27
#define FLIR_UP_ARROW        28
#define FLIR_RIGHT_ARROW     29
#define FLIR_DOWN_ARROW      30
#define FLIR_LEFT_ARROW      31
#define FLIR_DIR_POINTER     32
#define RDR_RIGHT_CARET      33
#define RDR_FILLED_CIRCLE    34
#define HI_RES_MAP_TARGET    35
#define GROUND_TARGET_SYMBOL 36
#define TACAN_SYMBOL         37
#define SHOOTQ_SYMBOL        38
#define FLIR_CENTER          39
#define LASE_SYMBOL          40
#define MASK_SYMBOL          41

#define TEWS_AIRCRAFT0       42
#define TEWS_AIRCRAFT1       43
#define TEWS_AIRCRAFT2       44
#define TEWS_AIRCRAFT3       45
#define TEWS_AIRCRAFT4       46
#define TEWS_AIRCRAFT5       47
#define TEWS_AIRCRAFT6       48
#define TEWS_AIRCRAFT7       49
#define TEWS_AIRCRAFT8       50
#define TEWS_AIRCRAFT9       51
#define TEWS_AIRCRAFT10      52
#define TEWS_AIRCRAFT11      53
#define TEWS_AIRCRAFT12      54
#define TEWS_AIRCRAFT13      55
#define TEWS_AIRCRAFT14      56
#define TEWS_AIRCRAFT15      57

#define TEWS_RADAR           58
#define TEWS_AAA             59
#define TEWS_SAM             60
#define TEWS_MISSLE0         61
#define TEWS_MISSLE1         62
#define TEWS_MISSLE2         63
#define TEWS_MISSLE3         64
#define TEWS_MISSLE4         65
#define TEWS_MISSLE5         66
#define TEWS_MISSLE6         67
#define TEWS_MISSLE7         68
#define TEWS_MISSLE8         69
#define TEWS_MISSLE9         70
#define TEWS_MISSLE10        71
#define TEWS_MISSLE11        72
#define BULLSEYE_SYMBOL      73
#define STAR_OF_DAVID        74

// BEGINS F18 SYMBOLS
#define AV_AA_CLOSED_STAR    75
#define AV_AA_CLOSED_DIAMOND 76
#define AV_AG_BACKGROUND     77
#define AV_ADI_BACKGROUND    78



#define SEARCH_MODE				0
#define SGL_TRT_MODE			1
#define DTWS_MODE				2
#define NDTWS_MODE				3
#define VCTR_MODE				4
#define	VS_MODE					5
#define SUPER_SEARCH_MODE       6
#define BORE_SIGHT_SHORT_MODE   7
#define BORE_SIGHT_LONG_MODE    8
#define VERT_SCAN_MODE			9
#define GUN_RDR_MODE		    10

void DrawRadarSymbol(int ScrX, int ScrY, int SymbolId, int Color, GrBuff *Buff);
void DrawAvRadarSymbol(int ScrX, int ScrY, int SymbolId,int Red,int Green,int Blue, GrBuff *Buff);

int  IsPlaneDetected(int PlaneId);
int  GetAARadarSubMode(void);
void DrawRadarSymbolKeepColors(int ScrX, int ScrY, int SymbolId);
void DrawRadarSymbol(int ScrX, int ScrY, int SymbolId, int Color, GrBuff *Buff);
void Aim7Fired();
PlaneParams *GetTargetForAmraam();
void SetAutoAcq(int Mode);
void CleanupRadarStuff(void);
void ResetAARadarToSearchMode();
void  InitRadar(void);
void SetIff(int State);
int GetCurrentAATargetIndex();


#endif