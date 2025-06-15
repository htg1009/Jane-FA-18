#ifndef __MEMMAP_FILE_HEADER__
#define __MEMMAP_FILE_HEADER__

class __declspec(dllexport) CMemMap
{
private:				// data
	HANDLE				m_hFile;
	HANDLE				m_hMapping;
	BOOL				m_bWritable;
	char *				m_pszFile;
	char *				m_pRawData;
	DWORD				m_dwMapFileSize;
						
protected:				// methods
	void				ResetVars (void);
	void				CloseFile (void);
	BOOL				MapFile (char *pszFile, BOOL bWritable, BOOL bCreate, BOOL bOverwrite, DWORD dwSize = 0);
	void				UnMapFile (void);
//	BOOL				ExtendFileSize (DWORD dwNewSize);

public:					
						CMemMap (void);
						~CMemMap (void);

	HANDLE				GetMapHandle (void) { return (m_hMapping); };
	LPVOID				GetPointer (void);
	BOOL				Create (char *pszFile, BOOL bOverwrite, DWORD dwSize);
	BOOL				Open (char *pszFile, BOOL bWritable);
	void				Close (void);
};

#endif
