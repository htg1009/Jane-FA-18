#ifndef __CAT_HEADER__
#define	__CAT_HEADER__

/////////////////////////////////////////////////////
// Private Definitions
/////////////////////////////////////////////////////
#define	INVALID_CAT_ORDINAL		(-1)
#define	BAD_SEEK				(0xffffffff)
#define	MAX_CMP_FILES			4096

/////////////////////////////////////////////////////
// Private function prototypes
/////////////////////////////////////////////////////
int ExpandMemToMem (PBYTE pCompressedBuffer, UINT nCompressedSize, PBYTE pUncompressedBuffer, UINT BuffSize, ULONG *pulCrc);
int CompressFileToFile(char *pszDest, char *pszSrc, ULONG *pulCrc, UINT *pnCompressedSize);
int ExpandFileToFile(char *pszDest, char *pszSrc);

/////////////////////////////////////////////////////
// Private data structures used by the CCat object
/////////////////////////////////////////////////////
typedef struct 
{
	char		szFile[64];
	UINT		unNativeSize;
	UINT		unPackedSize;
	DWORD		dwOffset;
	DWORD		dwCrc;
	int			nOrdinal;
	LPVOID		pNativeMem;
	BOOL		bCompressed;
} FILEHDR, *PFILEHDR;

typedef struct 
{
	DWORD	dwEntries;
	DWORD	dwReserved1;
	DWORD	dwReserved2;
	DWORD	dwReserved3;
	DWORD	dwReserved4;
} CATHDR, *PCATHDR;

/////////////////////////////////////////////////////
// Class definition
/////////////////////////////////////////////////////
class __declspec(dllexport) CCat 
{
private:
	char		m_szFileName[260];
	HANDLE		m_hFile;
	PFILEHDR 	m_pHdrList;
	CATHDR		m_CatHdr;

protected:
	PFILEHDR	GetFileHdr (char *pszFile);
	PFILEHDR	GetFileHdr (int nOrdinal);
	int			LoadFile (PFILEHDR pHdr);
	BOOL		CacheFile (PFILEHDR pHdr);
	BOOL		FreeFile (PFILEHDR pHdr);

public:
	CCat		(void);
	~CCat		(void);

	BOOL		Open (char *pszCatalogFile);
	void		Close (void);
	int			GetFileCount (void);
	char *		GetFileName (int nOrdinal);

	int			LoadFile (char *pszFile, LPVOID *ppDataPtr, DWORD *pdwSize);
	int			LoadFile (int nOrdinal, LPVOID *ppDataPtr, DWORD *pdwSize);
	BOOL		FreeFile (char *pszFile);
	BOOL		FreeFile (int nOrdinal);
	BOOL		WriteFileToDisk (int nOrdinal, char *pszFullPath, BOOL bOverwrite = TRUE);

	static BOOL	ExpandFile (char *pszDest, char *pszSource);
	static BOOL	CompressFile (char *pszDest, char *pszSource);
	static BOOL	IsCompressed (char *pszFile);

	BOOL		LoadAll (void);
	void		FreeAll (void);

	// Catalog creation function...
	// Prompts user for files to compress as well as catalog filename
	BOOL		CreateCatalog (void);
};

#endif