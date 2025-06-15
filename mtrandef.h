struct stMovieTransLineType
{
	int iID;
	int iFlags;						
	unsigned long lTime;			// Seconds or Frames
	unsigned long lDuration;		// Seconds or Frames
	unsigned char *pLineBuffer;				// Text Line Buffer
	long lLineBufferSize;			// Size of Text Line Buffer
	int iReserved[5];
};
typedef struct stMovieTransLineType MovieTransLineType;

struct stMovieTransType
{
	char szMovieName[260];			
	int iLanguage;
	int iFlags;
	int iNumTextLines;
	MovieTransLineType *pMovieLineList;
	int iReserved[5];
};
typedef struct stMovieTransType MovieTransType;
