//*****************************************************************************************
//   JOYSTUFF.H
//*****************************************************************************************

//*****************************************************************************************
//  Prototypes for JOYSTUFF.CPP
//*****************************************************************************************

extern "C" __declspec( dllexport ) int  InitJoy(void);
extern "C" __declspec( dllexport ) void ReadJoy(void);

void  InitMouse(void);
void ShutdownMouse();

void  ReadJoyWD(void);
void  ReadJoyVarD(DWORD dwDZone);

int   GetJoyPosX(void);
int   GetJoyPosY(void);
int   GetJoyPosR(void);

DWORD GetJoyDeadX(void);
DWORD GetJoyDeadY(void);
DWORD GetJoyDeadR(void);

DWORD GetJoyX(void);
DWORD GetJoyY(void);
DWORD GetJoyZ(void);
DWORD GetJoyR(void);
DWORD GetJoyU(void);
DWORD GetJoyV(void);

DWORD GetJoyPOV(void);

DWORD GetJoyB1(void);
DWORD GetJoyB2(void);
DWORD GetJoyB3(void);
DWORD GetJoyB4(void);

DWORD GetJoyButtons(void);
DWORD GetJoyBtn(DWORD dwButtonNum);

void GetJoyStickExtentsInPercent(int *IsLeft, int *IsUp, float *HorzPercent, float *VertPercent);
float GetJoystickDirection(void);


void  IncDeadX(void);
void  IncDeadY(void);
void  IncDeadR(void);
void  DecDeadX(void);
void  DecDeadY(void);
void  DecDeadR(void);

void  ReadKeyboard(void);
void  ReadMouse(void);
void  ReadMouseRelative(void);

LONG  GetMouseX(void);
LONG  GetMouseY(void);
short GetMouseLB(void);
short GetMouseRB(void);
short GetMouseMB(void);
LONG  GetMouseDX(void);
LONG  GetMouseDY(void);

