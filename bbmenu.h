struct stControlHandler
{
	LRESULT (*InputHandler)(HWND,UINT,WPARAM,LPARAM);
	void (*IdleLoop)();
	void (*MouseHandler)();
	int iDone;
	int iReturnCode;
};

typedef struct stControlHandler MenuHandler;

#define TERMINATE_GAME		0x0001
#define	HELP				0x0002
#define EXIT_MENU			0x0003
#define QUICK_START			0x0010
#define SINGLE_MISSION		0x0020

struct stReturnType
{
	short wParam;
	int iReturnCode;
};

typedef struct stReturnType ReturnCode;

struct stButton
{
	int iTextLayout;
	int iButtonX;
	int iButtonY;
	int iBigTextX;
	int iBigTextY;
	int iSmallTextX;
	int iSmallTextY;
	int iSelBoxX;
	int iSelBoxY;
	int iHotX;
	int iHotY;
	int iHotWidth;
	int iHotHeight;
	int iHotSpotId;
	char ButtonDesc[30];
};

typedef struct stButton ButtonType;

struct stPoint
{
	float xp;
	float yp;
};

typedef struct stPoint PointList;

struct stIntPoint
{
	int x;
	int y;
};

typedef struct stIntPoint Point;


extern MenuHandler ControlHandler;
