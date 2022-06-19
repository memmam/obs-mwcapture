#ifndef MWCMDCOMMON_H
#define MWCMDCOMMON_H

#include "Windows.h"
#include <gdiplus.h>
#include <direct.h>

using namespace Gdiplus;

//A2W
inline wchar_t* AnsiToUnicode( const char* szStr )
{
	int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0 );
	if (nLen == 0)
	{
		return NULL;
	}
	wchar_t* pResult = new wchar_t[nLen];
	MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen );
	return pResult;
}

// W2A
inline char* UnicodeToAnsi( const wchar_t* szStr )
{
	int nLen = WideCharToMultiByte( CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL );
	if (nLen == 0)
	{
		return NULL;
	}
	char* pResult = new char[nLen];
	WideCharToMultiByte( CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL );
	return pResult;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

BOOL 
GetPath(char* szPath,int nSize)
{
	char szTmp[256];
	char* pszTmp = NULL;
	size_t tSZ = 0;
	if(_dupenv_s(&pszTmp,&tSZ,"HOMEDRIVE")==0&&
		pszTmp!=NULL){
			strcpy_s(szTmp,pszTmp);
			free(pszTmp);
			pszTmp = NULL;
	}else
		return FALSE;

	if(_dupenv_s(&pszTmp,&tSZ,"HOMEPATH")==0&&
		pszTmp!=NULL){
			strcat_s(szTmp,pszTmp);
			free(pszTmp);
			pszTmp = NULL;
	}else
		return FALSE;

	strcat_s(szTmp,"\\Magewell");
	if(nSize<256)
		return FALSE;
	memcpy(szPath,szTmp,256);

	return TRUE;
}

#endif