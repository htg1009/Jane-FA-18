/////////////////////////////
// MS API
//
// (c) Electronic Arts, 1997
//
#define		MS_NO_ERROR						TRUE
#define		MS_ERROR_SERVER_NOT_FOUND		1000
#define		MS_ERROR_UNKNOWN_SERVER			1001
#define		MS_ERROR_SOCKET_ERROR			1002
#define		MS_LOST_SERVER_CONNECTION		1003
#define		MS_ERROR_WINSOCK				1004
#define		MS_DATA_ERROR					1005
#define		MS_NO_MATCHES					1006
#define		MS_LOGIN_INCORRECT				1007
#define		MS_HOST_ALREADY_SELECTED		1008
#define		MS_UPLOAD_ERROR					1009
#define		MS_UPLOAD_NOTALLOWED			1010
#define		MS_DATEFILESIZE_ERROR			1011
#define		MS_DATAFILETRANSFER_ERROR		1012


typedef struct 
{
	int totalintfields;			// Cannot be greater than 64 for this version
	int *intfieldlengths;
	int *intfieldweights;
	int *intfieldvariance;

	int totalcharfields;
	int *charfieldlengths;
	int *charfieldweights;
	int *charfieldvariance;
} MSFILTER;

typedef struct 
{	
	short		sin_family;	
	u_short		sin_port;	
	struct		in_addr	sin_addr;
	int			latency;	
	int			unused;	
	int			refnum;
} MSIPINFO;

typedef void (* MSPlayerWaiting) (MSIPINFO *playerinfo, void * playerdata);

extern "C"
{
	__declspec( dllexport ) int connectMS (char *RegistryKey);

	__declspec( dllexport ) int initializeMS (void *playerdata, MSFILTER *filter, int playerdatasize, int CheckPeriod);

	__declspec( dllexport ) int loginMShost(MSPlayerWaiting CallbackFunction);

	__declspec( dllexport ) int loginMSPlayer();

	__declspec( dllexport ) int requestMSgame (MSIPINFO *hostIPinfo, void *hostplayerdata);

	__declspec( dllexport ) int resetMSfilter();

	__declspec( dllexport ) int selectMSgame (MSIPINFO *hostIPinfo);

	__declspec( dllexport ) int deselectMSgame (MSIPINFO *hostIPinfo);

	__declspec( dllexport ) int updateMSgame (void *playerdata);

	__declspec( dllexport ) int fetchMSgame (MSIPINFO *hostIPinfo, void *playerdata);	

	__declspec( dllexport ) int sendMSresults (void *results, char *otherdata, int lenotherdata);

	__declspec( dllexport ) int getMSdatafilesize (char *DFID, int *DFSize);
	
	__declspec( dllexport ) int getMSdatafile (char *DFID, char *filedata, int lenfiledata);

	__declspec( dllexport ) int closeMS();
};

int MSRead(int sock, char *buf, int len);

int MSSend(int sock, char *buf, int len);

int MSReadInt(int sock, int *var);

int MSSendInt(int sock, int *var);

UINT TimerFunc( LPVOID pParam );

int MSSendFilter(MSFILTER *filter);
