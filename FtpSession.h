#ifndef __FTP_SESSION_HEADER__
#define __FTP_SESSION_HEADER__

#include <dplay.h>
#include "MultiMsg.h"
#include "FtpPacket.h"

extern  int			g_nFtpFileFlags;

#define	FTPMODE_TX				1
#define FTPMODE_RX				2

#define	FILETYPE_MISSION				0x0001
#define FILETYPE_RSC					0x0002
#define FILETYPE_DAT					0x0003
#define FILETYPE_PILOT					0x0004
#define FILETYPE_SQUADRON				0x0005
#define	FILETYPE_MISSION_UPDATE			0x0006
#define FILETYPE_SHIP_DAMAGE			0x0007
#define FILETYPE_GROUND_DAMAGE			0x0008
#define FILETYPE_MISC					0xFFFF

#define FTP_SEND_PILOT					0x0001
#define FTP_SEND_SPEECH					0x0002
#define FTP_SEND_SQUADRON				0x0004
#define FTP_RECV_PILOT					0x0008
#define FTP_RECV_SPEECH					0x0010
#define FTP_RECV_SQUADRON				0x0020

#define FTPMGR_DATA						0
#define	FTPMGR_INIT						1
#define	FTPMGR_INIT_ACK					2
#define FTPMGR_PACKET_REQ				3
#define FTPMGR_RX_COMPLETE				4
#define FTPMGR_CANCEL					5
#define FTPMGR_QUERY_FILE_FLAGS			6
#define FTPMGR_FILE_FLAGS				7
#define FTPMGR_LASTMSG					FTPMGR_FILE_FLAGS

#define FTPSTATE_NONE					0
#define FTPSTATE_TX_WAITING_INIT_ACK	1
#define FTPSTATE_TX						2
#define FTPSTATE_RX						3
#define FTPSTATE_ABORTED				4
#define FTPSTATE_COMPLETED				5

#define FTPERR_SUCCESS					0
#define FTPERR_FILE_NOT_FOUND			1
#define FTPERR_TIMEOUT					2
#define FTPERR_REMOTE_DISCONNECT		3
#define FTPERR_BAD_FILE_PTR				4

typedef struct
{
	int					nType;
	int					nFlags;
	int					nReserved;
	char				szPath[128];
	char				szFile[128];
} FTPNEWFILE;

class CFtpSession
{
public:
	char				m_szFile[260];
	char				m_szRemotePlayer[60];
	char				m_szLocalPlayer[60];
	HANDLE				m_hFile;
	HANDLE				m_hLogFile;
	int					m_nMode;
	int					m_nState;
	int					m_nBlockSize;
	int					m_nFileSize;
	int					m_nDuplicates;
	int					m_nLastChunk;
	int					m_nNextChunk;
	int					m_nWaitingOnChunk;
	BYTE *				m_pbyChunkList;
	int					m_nLocalSession;
	int					m_nRemoteSession;
	int					m_nChunksXfer;
	int					m_nResends;
	int					m_nDebug;
	int					m_nFileType;
	int					m_nChunkTimeouts;
	int					m_nLastChunkWritten;
	CFtpSession *		m_pNext;
	DPID				m_dpidLocal;
	DPID				m_dpidRemote;
	HWND				m_hWndNotify;
	DWORD				m_dwRequestedChunkTime;
	DWORD				m_dwTimeLastChunkSent;
	BOOL				m_bFinished;

protected:
	void				Reset (void);
	void				SetState (int nNewState);

public:
	// General purpose functions
						CFtpSession (void);
						~CFtpSession (void);
	int					InitTx (HWND hWndNotify, int nSession, DPID dpID, char *pszFile, DPID dpidRemote, char *pszRemote, int nFileType);
	void				InitRx (HWND hWndNotify, int nSession, DPID dpID, FTPPACKET *pPacket);
	void				SendMsg (int nMsg, int nValue, void *pData = NULL, int nSize = 0);
	void				ReceiveMsg (FTPPACKET *pPacket);
	void				CloseFile (BOOL bKeepFile);
	void				OnCancel (FTPPACKET *pPacket);
	void				Timeout (void);
	BOOL				FtpSuccessful (void);	// did it complete?
	BOOL				FtpDone (void);			// has it stopped? (could be aborted or completed)
	BOOL				FtpAborted (void);		// did it abort?

	// Transmit functions
	void				TxProcess (void);
	void				TxSendSpecificChunk (int nChunk);
	BOOL				TxReadChunk (int nChunk, void *pBuffer, LPDWORD pdwSize);

	// Receive functions
	void				RxWriteChunk (FTPPACKET *pPacket);
	void				RxProcess (void);
	int					DetermineNextRequestedChunk (void);
	void				ReceiveComplete (void);
	void				CheckForCompletion (void);
};

// Used to replace a pointer where we don't want an FTP session. Should obviously not be referenced as an actual pointer!
#define NO_FTP_REQUIRED	((CFtpSession *)(0xFFFFFFFF))

#endif