#ifndef __SKUNKSND_HEADER__
#define __SKUNKSND_HEADER__

#include "SkunkSndDefs.h"
//#include "fmath.h"

/************************************************************************************************************
* BUILDING SOUND FUNCTIONALITY INTO YOUR PROGRAM
*
*	1) Add SkunkSnd.lib to your project
*	2) Copy SkunkSnd.dll to your executable's directory
*	3) Create sound catalog using SndCat.exe program
*	4) Move sound catalog to directory used by your program
************************************************************************************************************/

/************************************************************************************************************
* SOUND FUNCTIONS IN A NUTSHELL
*
*	1) Initialize sound engine
*		BOOL bInit = SndInit ();
*
*	2) Open the sound catalog file for playing:
*		BOOL bOk = SndOpenVolume ("C:\\Janes\\Sounds\\F15.rwv");
*
*	3) Queue sound to play
*		DWORD dwHandle = SndQueueSound (SOUND_ID_ENGINE, 0);	// 0 for 2nd parm means loop infinite
*
*	4) Stop sound (specific sound or all sounds)
*		SndEndSound (dwHandle);
*			or
*		SndEndAllSounds ();
*
*	5) Close sound catalog file
*		SndCloseVolume ();
*
*	6) Free sound engine
*		SndFree ();
*
*	Thats it! If you want to modify or query your sounds, check the functions below- note that they all
*		use the dwHandle returned from the SndQueueSound () function.
************************************************************************************************************/

/********************************************************************************
* OBJECT ORIENTED ALTERNATIVE........
*
*	If you prefer OOP style coding, you can simply create a C++ object instance
*	of the CSoundObj class, and call its member functions directly...
*	The member functions match the functions in SkunkSnd.h exactly; the only 
*	difference is that the member functions don't have the "Snd" prefix in their names.
********************************************************************************/
 
/*******************************************************************************
 * Channel definitions
 * Note that general sound effects are played on general purpose channels...
 *	Don't specify these special purpose channels with general purpose sound effects
 ******************************************************************************/
#define	CHANNEL_GENERIC		(-1)
#define	CHANNEL_BETTY		0
#define	CHANNEL_WSO			1
#define	CHANNEL_WINGMEN		2
#define CHANNEL_SPEECH		3
#define	CHANNEL_ENGBASS		4
#define	CHANNEL_INTAKE_L	5
#define	CHANNEL_INTAKE_R	6
#define	CHANNEL_WNDNOISE	7

/******************************************************************************
 * Initialization & shutdown routines - must be called first & last, respectively
 *****************************************************************************/
__declspec(dllexport) BOOL	SndInit (DWORD dwRate = 11025, WORD wBits = 8, WORD wChannels = 1);
__declspec(dllexport) BOOL	SndFree (void);

/******************************************************************************
 * Raw Wave Volume (RWV) File Operations (Open & Close Sound Catalog)
 *****************************************************************************/
__declspec(dllexport) BOOL	SndOpenVolume (char *pszFile);	// pszFile = complete path of sound catalog file
__declspec(dllexport) void	SndCloseVolume (void);

/*******************************************************
 * Timer function - must be called frequently by game! (Once through message loop)
 *******************************************************/
__declspec(dllexport) void	SndServiceSound (void);	

/**********************************************************************************************************
 * Sound playing function:
 **********************************************************************************************************
 *	SndQueueSound (DWORD dwID, int nLoopCount = -1, int nVolume = VOLUME_UNSPECIFIED, int nChannel = -1);  
 *
 *	PARMS:
 *		dwID: sound id, typically a #define in sound header file
 *		nLoopCount: NOT REQUIRED PARAMETER! -1 = default (once); 0 = infinite looping; 1 or more = number of repetitions
 *		nVolume: NOT REQUIRED PARAMETER! 0 = softest, 127 = loudest
 *		nChannel: NOT REQUIRED PARAMETER! Specifies channel (0-15); Use with extreme discretion!
 *		nPlaybackRate: Frequency at which sample will be played back. Useful for sound effect shifts
 *
 *	RETURNS:
 *		HANDLE for sound. Keep the return value in case you need to query or modify its status later on!
 **********************************************************************************************************/
__declspec(dllexport) DWORD	SndQueueSound (DWORD dwID, int nLoopCount = -1, int nVolume = VOLUME_UNSPECIFIED, int nChannel = -1, int nPlaybackRate = -1, int nPan = PAN_MIDDLE);  


/***************************************
 * Sound stopping functions
 **************************************/
__declspec(dllexport) int		SndEndSound (DWORD dwHandle);		// Kills specific sound (if playing)
__declspec(dllexport) void		SndEndAllSounds (void);				// Kills ALL sounds
__declspec(dllexport) void		SndStopChannel (int nChannel);		// Stops sound playing on specified channel

/***************************************
* Query function
***************************************/
__declspec(dllexport) int		SndIsSoundPlaying (DWORD dwHandle);	// 0 indicates sound not playing
__declspec(dllexport) void		SndDumpStatus (void);				// Dumps diagnostics to output window
__declspec(dllexport) void		SndDiagnose (BOOL bDiagnose);		// Enables diagnostic data dump
__declspec(dllexport) void *	SndGetDirectSound (void);

/**************************************
 * Sound modification functions
 *************************************/
__declspec(dllexport) int		SndSetSoundVolume (DWORD dwHandle, int nVolume);	// volume range: 0 (quiet) to 127 (loudest)
__declspec(dllexport) BOOL		SndSetLoopCount (DWORD dwHandle, int nLoops);		// loops: 0 = infinite, any other value = count
__declspec(dllexport) int		SndSetSoundPlaybackRate (DWORD dwHandle, int nRate);// rate: 8000 to 44100; the higher the rate, the higher the pitch
__declspec(dllexport) int		SndSetSoundPanPosition (DWORD dwHandle, int nPan);	// pan: 0 = left speaker, 64 = balanced, 127 = right speaker
//__declspec(dllexport) int		SndSetSound3DPosition (DWORD dwHandle, FPointData pos);	// pos: position in 3d world
//__declspec(dllexport) void	SndSetUserPosition (FPointData Pos3d);					// pos3d: user (camera) position in 3d space

/******************************
 * Pausing/Resuming functions
 *****************************/
__declspec(dllexport) int		SndPauseSound (DWORD dwHandle);		// Pause specific sound
__declspec(dllexport) int		SndResumeSound (DWORD dwHandle);	// Resume specific sound
__declspec(dllexport) void		SndPauseAllSounds (void);				
__declspec(dllexport) void		SndResumeAllSounds (void);

/**************************
 *	Speech functions
 *************************/
__declspec(dllexport) DWORD		SndQueueSentence (int nChannel, int nNumChunks, int *pnChunkIds, int nVolume = -1, int nPan = - 1);
__declspec(dllexport) DWORD		SndQueueSpeech (DWORD dwID, int nChannel, int nVolume = -1, int nLoops = -1, int nFrequency = -1);	// Queue speech sound byte
__declspec(dllexport) void		SndFlushSpeechQueue (int nChannel);													// Flush entire speech channel
__declspec(dllexport) void		SndEndAllSpeech (void);																// Nuke ALL speech on ALL channels

/*****************************************************
 * Streaming sound play function
*****************************************************/
__declspec(dllexport) BOOL SndStreamInit (char *pszWaveFile);
__declspec(dllexport) void SndStreamStop (void);
__declspec(dllexport) void SndStreamClose (void);
__declspec(dllexport) void SndServiceStream (void);
__declspec(dllexport) void SndStreamPlay (void);
__declspec(dllexport) void SndStreamPause (void);
__declspec(dllexport) void SndStreamResume (void);
__declspec(dllexport) void SndStreamSetPlaybackRate (int nPlaybackRate);
__declspec(dllexport) void SndStreamSetVolume (int nNewVolume);
__declspec(dllexport) void SndStreamSetPan (int nPan);
__declspec(dllexport) void SndStreamSetLoopCount (int nCount);
__declspec(dllexport) BOOL SndStreamIsPlaying (void);
__declspec(dllexport) int SndStreamGetVolume (void);

#endif
