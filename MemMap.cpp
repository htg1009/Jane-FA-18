//#ifdef __STRIKE_TOOL__
#include <stdafx.h>
//#endif

#include <crtdbg.h>
#include <stdio.h>
#include "MemMap.h"

#define		FILE_OPEN()					(m_pRawData != NULL)

void CMemMap::ResetVars (void)
{
	m_hMapping = NULL;
	m_bWritable = FALSE;
	m_pszFile = NULL;
	m_pRawData = NULL;
	m_dwMapFileSize = 0;
}

LPVOID CMemMap::GetPointer (void)
{
	if (FILE_OPEN())
		return (m_pRawData);
	else
		return (NULL);
}

CMemMap::CMemMap (void)
{
	ResetVars ();
}

CMemMap::~CMemMap (void)
{
	Close ();	
}

BOOL CMemMap::Create (char *pszFile, BOOL bOverwrite, DWORD dwSize)
{
	Close ();
	return (MapFile (pszFile, TRUE, TRUE, bOverwrite, dwSize));
}

BOOL CMemMap::MapFile (char *pszFile, BOOL bWritable, BOOL bCreate, BOOL bOverwrite, DWORD dwSize)
{
	BOOL					bOk = FALSE;
	SECURITY_ATTRIBUTES		sa;

	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	// Close file first to write all changes to disk & start from 
	Close ();

	if (dwSize)
		m_dwMapFileSize = dwSize;
	else
		m_dwMapFileSize = 0;

	if (bCreate)
		m_hMapping = CreateFileMapping ((HANDLE)0xFFFFFFFF, &sa, PAGE_READWRITE, 0, m_dwMapFileSize, "VIBESEAT");
	else
		m_hMapping = OpenFileMapping (FILE_MAP_READ, TRUE, "VIBESEAT");

	if (m_hMapping != NULL)
	{
		m_pRawData = (char *) MapViewOfFile (m_hMapping, bWritable ? FILE_MAP_WRITE : FILE_MAP_READ, 0, 0, 0);
		if (m_pRawData)
		{
			bOk = TRUE;
			m_bWritable = bWritable;
		}
	}

	return (bOk);
}

BOOL CMemMap::Open (char *pszFile, BOOL bWritable)
{
	if (!pszFile)
		return (FALSE);
	else
		Close ();

	return (MapFile (pszFile, bWritable, FALSE, FALSE));
}

void CMemMap::UnMapFile (void)
{
//	DWORD	dwSize = 0;

	if (FILE_OPEN())
	{
//		NOT NECESSARY SINCE OUR FILESIZE ISN'T DYNAMIC
//		Get size data while file is still mapped
//		if (m_bWritable)
//			dwSize = GetCurrentFileSize ();

		if (m_pRawData)
			FlushViewOfFile (m_pRawData, 0);

		UnmapViewOfFile (m_pRawData);

		if (m_hMapping)
			CloseHandle (m_hMapping);
	}
}

void CMemMap::Close (void)
{
	if (FILE_OPEN())
		UnMapFile ();

	ResetVars ();
}

