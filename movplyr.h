// ======================================================================
//
// movplyr.h
// jeff grills, martin griffths, Hideki Ikeda
//
// copyright 1997 origin systems inc
//
// ======================================================================

#ifndef MOVPLYR_H
#define MOVPLYR_H

// ======================================================================
#include "uvstruct.h"
#include "bit.h"
// #include "dirsound.h"
#include <mmsystem.h>
#include <dsound.h>
#include "mtrandef.h"
#include "TrueTypeFont.h"

class Font;
class LanguageSection;

#define		UV_MOVIEDONE		(WM_USER+1031)

// ======================================================================

class __declspec(dllexport) MoviePlayer
{
public:

	MoviePlayer(LPDIRECTDRAWSURFACE4 pSurf, LPDIRECTDRAWSURFACE4 pBakSurf,LPDIRECTSOUND pSound, const char *fileName, int flags=FlagDefault, int x=0, int y=0, void *hWndParent = NULL);
	~MoviePlayer(void);

	enum
	{
		FlagCenter     = BINARY2(0000,0001),
		FlagDoubleX    = BINARY2(0000,0010),
		FlagSkipY      = BINARY2(0000,0100),
		FlagDoubleY    = BINARY2(0000,1000),
		FlagLooping    = BINARY2(0001,0000),
		FlagSoundLimit = BINARY2(0010,0000),
		FlagAsyncLoad  = BINARY2(0100,0000),

		FlagDefault    = FlagCenter | FlagDoubleX | FlagSkipY | FlagSoundLimit
	};

protected:

	enum Status
	{
		Playing,
		Paused,
		Loading,
		Stopped
	};

	enum ChunkType
	{
		Error,
		Junk,
		Eof,
		Audio,
		Video,
		Valid,
		Full
	};

	enum
	{
		MAX_FILES    = 2,
		QUEUE_SIZE   = 64,
		QUEUE_MASK   = QUEUE_SIZE - 1,
	};

	struct AppendList
	{
		int         file;
		AppendList *next;
	};

	struct QueueEntry
	{
		ChunkType  type;
		u_char    *data;
		int        size;
	};

	typedef LPDIRECTSOUNDBUFFER LPDSB;

protected:

	real                       volume;
	static int                 gammaCorrection;

protected:

	char        *videoName;
	char        *audioName;
	AppendList  *appendList;

	int          file[MAX_FILES];

	Status       status;

	int          splitRead;
	BOOL         splitAudioVideo;
	BOOL         hardSoundSync;
	BOOL         looping;
	BOOL         eof;
	BOOL         asyncLoad;

	BOOL         audioPresent;
	BOOL         audioStarted;
	int          audioRate;
	int          audioChannels;
	int          audioCompressedChannels;
	int          audioDepth;
	int          audioBufferSize;
	int          audioRequired;
	int          audioFilled;
	int          audioFillCalls;
	int          audioAdjust;
	int          audioSamples;
	int          audioSampleSize;
	int          audioWritePosition;
	LPDSB        audioStreamBuffer;

	u_char      *videoData;
	int          videoDropped;
	int          videoDrawn;
	int          videoDecoded;

	int          currentFrame;

	QueueEntry   queue[QUEUE_SIZE];
	int          queueVideo;
	int          queueAudio;
	int          queueTail;

	uv_instance  uv;

	Font        *subtitleFont;

	u_int8      *memoryPool;
	u_int8      *readPool;
	u_int8      *audioPool;
	u_int8      *audioPoolHead;
	u_int8      *audioPoolTail;
	u_int8      *videoPool;
	u_int8      *videoPoolHead;
	u_int8      *videoPoolTail;

	int          asyncSkip;

	BOOL         readPoolValid;
	LPDIRECTDRAWSURFACE4   pSurf;
	LPDIRECTSOUND		pSound;
	HWND				hwndParent;
	BOOL		bEnabled;
	DWORD		dwLastFrame;

	unsigned int	iTickCount;
	CTrueTypeFont *		m_pFont;
	void *	m_pRawSurf;
	LPDIRECTDRAWSURFACE4   m_pBakSurf;
	MovieTransType *m_pMovieTransList;

protected:

	void      init(int flags, int x, int y);

	void      audioSetup(void *data, int len);
	int       audioBytesWritable(void);
	void      audioFill(void *data, DWORD len);
	void      audioStart(void);

	void      videoSetup(void);
	void      videoSetData(u_char *data);
	void      videoClearData(void);

	BOOL      queueFull(void) const;
	int       queueAdvance(int entry) const;
	void      queueFlush(void);
	void      queueLoad(BOOL hint);
	void      queueAddAudio(u_char *data, int size);
	void      queueAddVideo(u_char *data);

	ChunkType readPacket(int fileIndex);
	ChunkType processPacket(int fileIndex);

	u_int8   *newAudioBuffer(int size);
	void      deleteAudioBuffer(u_int8 *ptr);
	u_int8   *newVideoBuffer(int size);
	void      deleteVideoBuffer(u_int8 *ptr);

	void      drawDD(void);
	void	DrawSubTitles();
	void	InitTransMovieData();
	void	CleanTransData();
	void	InitTransData(char *sFileName);


public:
	void appendMovie(const char *fileName);

	void update(void);
	void draw(void);

	BOOL isLoading(void) const;
	BOOL isPlaying(void) const;
	BOOL isPaused(void) const;
	BOOL isStopped(void) const;
	int  getVideoSync(void) const;
	int  getCurrentFrame(void) const;
	int  getTotalFrames(void) const;
	void gotoFrame(int frameNum);

	void play(void);
	void pause(void);
	void stop(void);

	void setVolume(real newVolume);
	real getVolume(void);

	static void setGammaCorrection(int newGammaCorrection);
	static int  getGammaCorrection(void);
};

// ======================================================================

// ======================================================================

inline int MoviePlayer::queueAdvance(int i) const
{
	return ((i + 1) & QUEUE_MASK);
}

inline BOOL MoviePlayer::queueFull(void) const
{
	return (queueAdvance(queueTail) == queueVideo || (audioPresent && queueAdvance(queueTail) == queueAudio));
}

inline void MoviePlayer::videoSetData(u_char *newVideoData)
{
	if (videoData)
	{
		videoDropped++;
		deleteVideoBuffer(videoData);
	}

	videoData = newVideoData;
}

inline void MoviePlayer::videoClearData(void)
{
	deleteVideoBuffer(videoData);
	videoData = NULL;
}

inline void MoviePlayer::play(void)
{
	if (status == Paused)
		status = Playing;
}

inline void MoviePlayer::stop(void)
{
	status = Stopped;
}

inline BOOL MoviePlayer::isLoading(void) const
{
	return (status == Loading);
}

inline BOOL MoviePlayer::isPlaying(void) const
{
	return (status == Playing);
}

inline BOOL MoviePlayer::isPaused(void) const
{
	return (status == Paused);
}

inline BOOL MoviePlayer::isStopped(void) const
{
	return (status == Stopped);
}

inline int MoviePlayer::getCurrentFrame(void) const
{
	return currentFrame;
}

inline real MoviePlayer::getVolume(void)
{
	return volume;
}

// ======================================================================

#endif