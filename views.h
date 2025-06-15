
#ifndef __VIEWS_H
#define __VIEWS_H


#define CAMERA_COCKPIT  		1
#define CAMERA_CHASE 			2
#define CAMERA_FIXED_CHASE 		3
#define CAMERA_FLYBY			4
#define CAMERA_CINEMATIC		5
#define CAMERA_TACTICAL			6
#define CAMERA_REVERSE_TACTICAL	7
#define CAMERA_RC_VIEW			8
#define CAMERA_FREE             9

#define CF_TS_SET				1

typedef enum {
   CO_NADA = 0,
   CO_PLANE,            //1
   CO_VEHICLE,          //2
   CO_WEAPON,           //3
   CO_GROUND_OBJECT,    //4
   CO_POINT,			//5

   // New guys here

   ///////////////

   CO_MAX_TYPES
} CameraObjectType;

typedef enum {
	CAMERA_STATE_SIMPLE_RC,
	CAMERA_STATE_STATIC_VIEW,
	CAMERA_STATE_PARABOLIC,
   // New guys here

   ///////////////
   CAMERA_STATE_MAX_STATES
} FlybyState;


#define CF_SUN_VISIBLE	1

typedef struct _CameraInstance
{
	DWORD Flags;
	float CHASE_T;
	float RELATIVE_T;
	float RELATIVE_T_DIST;
	float DIST_T;
	DWORD RegisteredFlags;
	int CameraMode;
	int SubType;

	CameraObjectType AttachedObjectType;		//The subject of the view
	int *AttachedObject;
	int *LastAttachedObject;
	float SubjectRadius;
	FPointDouble SubjectLocation;
	FPointDouble LastSubjectLocation;
	FPointDouble SubjectPoint;
	FPoint SubjectVelocity;
	FMatrix SubjectMatrix;

	CameraObjectType TargetObjectType;		//the target of the view
	int *TargetObject;
	int *LastTargetObject;
	float TargetRadius;
	FPointDouble TargetLocation;
	FPointDouble LastTargetLocation;
	FPointDouble TargetPoint;
	FMatrix TargetMatrix;

	FPointDouble CameraLocation;
	FPointDouble CameraTargetLocation;
	FPointDouble LastCameraLocation;
	FMatrix CameraMatrix;
	FMatrix CameraQuatMatrix;
	Quat	CameraCurrentQuat;

	FMatrix CameraRelativeMatrix;
	FMatrix CameraRelativeQuatMatrix;
	Quat	CameraRelativeCurrentQuat;
	Quat	CameraRelativeToQuat;

	float DistanceFromObject;
	float TargetDistanceFromObject;

	FPoint MPD_VCPosition;
	FPoint MPD_VCdelta;
	float MPD_VCHeading;
	float MPD_VCPitch;

	FPoint MPD_MPDPosition;
	FPoint MPD_MPDdelta;
	float MPD_MPDHeading;
	float MPD_MPDPitch;

	float MPD_T;

	ANGLE Roll;
	ANGLE Pitch;
	ANGLE Heading;

	float CurrentRoll;
	float CurrentPitch;
	float CurrentHeading;

	float TargetRoll;
	float TargetPitch;
	float TargetHeading;

	/* ------------------------------------------2/8/98 11:00AM--------------------------------------------
	 * the following are used in the flyby and Cinematic camera modes
	 * ----------------------------------------------------------------------------------------------------*/

	FlybyState State;
	int BirthLoop;
	int TotalLoops;
	int LoopsLeft;
	double InitialX;
	double XStep;
	double ParabolaYOffset;
	double ParabolicConstant;
	double OneOverParabolicConstant;
	FPointDouble UpVelocity;
	FPointDouble TargetUpVelocity;
	FMatrix	ParabolaOrientation;

} CameraInstance;

#define COCKPIT_VIEW 0
#define FULL_SCREEN	 1

#define CC_MAKE_OFFSET_MAT		0x00000001
#define CC_USE_OFFSET_MAT		0x00000002
#define CC_USE_SUBJECT_MAT		0x00000004
#define CC_CONCAT_SUBJECT_MAT	0x00000008

#define CAMERA_UP_PRESSED					0x000000001
#define CAMERA_DOWN_PRESSED                 0x000000002
#define CAMERA_LEFT_PRESSED                 0x000000004
#define CAMERA_RIGHT_PRESSED                0x000000008
#define CAMERA_IN_PRESSED                   0x000000010
#define CAMERA_OUT_PRESSED                  0x000000020
#define CAMERA_IN_SLOW_PRESSED              0x000000040
#define CAMERA_OUT_SLOW_PRESSED             0x000000080
#define CAMERA_SLEW_NORTH_PRESSED           0x000000100
#define CAMERA_SLEW_NORTH_SLOW_PRESSED      0x000000200
#define CAMERA_SLEW_SOUTH_PRESSED           0x000000400
#define CAMERA_SLEW_SOUTH_SLOW_PRESSED      0x000000800
#define CAMERA_SLEW_EAST_PRESSED            0x000001000
#define CAMERA_SLEW_EAST_SLOW_PRESSED       0x000002000
#define CAMERA_SLEW_WEST_PRESSED            0x000004000
#define CAMERA_SLEW_WEST_SLOW_PRESSED       0x000008000
#define CAMERA_SLEW_UP_PRESSED            	0x000010000
#define CAMERA_SLEW_UP_SLOW_PRESSED       	0x000020000
#define CAMERA_SLEW_DOWN_PRESSED            0x000040000
#define CAMERA_SLEW_DOWN_SLOW_PRESSED       0x000080000
#define CAMERA_SLIDE_LEFT_PRESSED			0x000100000
#define CAMERA_SLIDE_LEFT_SLOW_PRESSED		0x000200000
#define CAMERA_SLIDE_RIGHT_PRESSED			0x000400000
#define CAMERA_SLIDE_RIGHT_SLOW_PRESSED		0x000800000
#define CAMERA_ZOOM_IN_PRESSED            	0x001000000
#define CAMERA_ZOOM_OUT_PRESSED            	0x002000000

extern DWORD ViewKeyFlags;

extern VKCODE vkUp;
extern VKCODE vkDown;
extern VKCODE vkLeft;
extern VKCODE vkRight;
extern VKCODE vkIn;
extern VKCODE vkOut;
extern VKCODE vkIn_slow;
extern VKCODE vkOut_slow;
extern VKCODE vkSlew_north;
extern VKCODE vkSlew_north_slow;
extern VKCODE vkSlew_south;
extern VKCODE vkSlew_south_slow;
extern VKCODE vkSlew_east;
extern VKCODE vkSlew_east_slow;
extern VKCODE vkSlew_west;
extern VKCODE vkSlew_west_slow;
extern VKCODE vkSlew_up;
extern VKCODE vkSlew_up_slow;
extern VKCODE vkSlew_down;
extern VKCODE vkSlew_down_slow;
extern VKCODE vkSlew_up;
extern VKCODE vkSlew_up_slow;
extern VKCODE vkSlew_down;
extern VKCODE vkSlew_down_slow;

extern VKCODE vkSlide_left;
extern VKCODE vkSlide_left_slow;

extern VKCODE vkSlide_right;
extern VKCODE vkSlide_right_slow;

extern VKCODE vkZoom_in;
extern VKCODE vkZoom_out;

void SetupView(int);
void CalculateCamera(CameraInstance *camera);

void NewCalculateCamera(CameraInstance *camera, PlaneParams *P);

void camera_setup_initial_relative_quats();
void camera_setup_relative_quats();

extern void view_up_pressed(BOOL held);
extern void view_down_pressed(BOOL held);
extern void view_left_pressed(BOOL held);
extern void view_right_pressed(BOOL held);
extern void view_in_pressed(BOOL held);
extern void view_out_pressed(BOOL held);
extern void view_in_slow_pressed(BOOL held);
extern void view_out_slow_pressed(BOOL held);
extern void view_slew_north_pressed(BOOL held);
extern void view_slew_north_slow_pressed(BOOL held);
extern void view_slew_south_pressed(BOOL held);
extern void view_slew_south_slow_pressed(BOOL held);
extern void view_slew_east_pressed(BOOL held);
extern void view_slew_east_slow_pressed(BOOL held);
extern void view_slew_west_pressed(BOOL held);
extern void view_slew_west_slow_pressed(BOOL held);
extern void view_slew_up_pressed(BOOL held);
extern void view_slew_up_slow_pressed(BOOL held);
extern void view_slew_down_pressed(BOOL held);
extern void view_slew_down_slow_pressed(BOOL held);
extern void view_slide_left_pressed(BOOL held);
extern void view_slide_left_slow_pressed(BOOL held);
extern void view_slide_right_pressed(BOOL held);
extern void view_slide_right_slow_pressed(BOOL held);
extern void view_zoom_in_pressed(BOOL held);
extern void view_zoom_out_pressed(BOOL held);

void SwitchCameraSubjectToPoint(FPointDouble &point,BOOL reset_ts = FALSE);
void SwitchCameraTargetToPoint(FPointDouble &point);

void AssignCameraSubject(void *sub,CameraObjectType type);
void AssignCameraTarget(void *sub,CameraObjectType type);
void ChangeViewModeOrSubject(DWORD to_mode);
extern void camera_setup_initial_distance(float dist);
extern void camera_setup_initial_location(FPointDouble &location);


#endif