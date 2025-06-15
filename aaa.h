#ifndef _AAA_H_
#define _AAA_H_

#define MAX_AAA_STREAM_VERTEXES 512

#define AAA_LOOPS_BETWEEN_BULLETS 	24
#define AAA_LOOPS_PER_SEGMENT		20
#define AAA_STREAM_DURATION			500
#define AAA_SLEW_GRAIN	0x150
#define AAA_LAG_LOOPS	100

#define AAA_BULLET_VELOCITY ((3380 FEET)/50.0)

#define AAA_INUSE			0x00000001
#define AAA_WORKHORSE		0x00000002
#define AAA_LEADING_TERM	0x00000004
#define AAA_TRAILING_TERM   0x00000008
#define AAA_ANCHORED_VERTEX 0x00000010
#define AAA_HAS_BEEN_SHOT	0x00000020
#define AAA_CALC_TS			0x00000040
#define AAA_NO_T			0x00000080
#define AAA_DETECT_HIT		0x00000100
#define AAA_NO_HEADER		0x00000200
#define AAA_NO_CHECK		0x08000000

typedef struct _AAAStreamVertex
{
	FPointDouble 	Position;
	FPointDouble	Vel;
	FPointDouble 	LastPosition;
	double			T1;
	double			Dt;
	int				TicksToT1;
	int				LagLoops;
	DWORD			MaxDuration;

	DBWeaponType		*Weapon;
	AAWeaponInstance	*Gun;

	ANGLE			Pitch;
	ANGLE			Heading;

	DWORD			BirthLoop;

	DWORD			Flags;

	struct _AAAStreamVertex *NextVertex;
	struct _AAAStreamVertex *NextStream;
	struct _AAAStreamVertex **PrevStream;
} AAAStreamVertex;

AAAStreamVertex *InstantiateAAAShots(AAWeaponInstance *gun,AAAStreamVertex *invert, ANGLE pitch, ANGLE heading, BOOL new_trailing_term = FALSE);
void MoveAAAStreamVertexes();
void DisplayAAAStreams(CameraInstance *camera);
void RenderAAASegment(AAAStreamVertex *from_vertex,FPoint &rel);

extern AAAStreamVertex AAAVertexes[],*LastAAAVertex,*AAAStreams;

#endif _AAA_H_