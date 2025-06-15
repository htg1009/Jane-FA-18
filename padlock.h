//*******************************************************************************
//*  padlock.h
//*  
//*  This file contains defines, types, prototypes, and globals 
//*  dealing with tracking padlocks.
//*******************************************************************************

//*******************************************************************************
//*  Bit Flags for lPadLockFlags
//*******************************************************************************

#define PAD_GLANCE	0x0001
#define PAD_LOST_TIME 15000

//*******************************************************************************
void PLGetTrackingVC();
void PLGetRelativeHP(PlaneParams *planepnt, FPointDouble worldposition, float *heading, float *pitch);
void PLGetRelativeHPR(PlaneParams *planepnt, FPointDouble worldposition, float *heading, float *pitch = NULL, float *roll = NULL);
int PLClipViewHP(float *heading, float *pitch, int clipdown = 0);
void PLWatchWSOWarn();
void PLGetNextClosestFriendly();
void PLWatchAGDesignated();
void PLGetNextObjectToCenter();

#ifdef SREPADLOCKSTUFF
void				*pPadLockTarget = NULL;
FPointDouble 		fpdPadLockLocation;
int					iPadLockType = 0;
void				*pLastWSOWarnObj = NULL;
int					iLastWSOWarnType = 0;
long				lPadLockTimer = -1;
long				lPadLockFlags = 0;
#else
extern void				*pPadLockTarget;
extern FPointDouble		fpdPadLockLocation;
extern int				iPadLockType;
extern void				*pLastWSOWarnObj;
extern int				iLastWSOWarnType;
extern long				lPadLockTimer;
extern long				lPadLockFlags;
#endif
