/************************************************************************************************/
// ReadWriteEDID.cpp : Defines the entry point for the console application.

// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell’s SDK 
// and not to Magewell’s SDK as a whole. 

// Copyrights © Nanjing Magewell Electronics Co., Ltd. (“Magewell”) All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell’s head files and library 
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or 
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL “AS IS” AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL MAGEWELL BE LIABLE 

// FOR ANY CLAIM, DIRECT OR INDIRECT DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT,
// TORT OR OTHERWISE, ARISING IN ANY WAY OF USING THE SOFTWARE.

// CONTACT INFORMATION:
// SDK@magewell.net
// http://www.magewell.com/
//
/************************************************************************************************/
#include "stdafx.h"
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <direct.h>

using namespace std;

#include <string.h>

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

#include "LibMWCapture\MWCapture.h"


HCHANNEL hChannel;
int		 *nProDevChannel;
int		 nChannelCount;

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void	 ShowInfo();
void	 Initialize();
BOOL	 WriteFilePermissionTest();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, char* argv[]);
void	 ReadWriteEDID(int argc, char* argv[]);
BOOL	 Getfilesuffix(const TCHAR* pFilePath);
void	 FileFailed(FILE* pFile);
void	 CloseDevice();
void	 ExitProgram();
wchar_t* AnsiToUnicode( const char* szStr );
char*	 UnicodeToAnsi( const wchar_t* szStr );
BOOL	 GetPath(char* szPath,int nSize);


int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler); 

	ShowInfo();

	Initialize();

	if(!WriteFilePermissionTest()){
		ExitProgram();
		MWCaptureExitInstance();
		return 1;
	}

	if(!ConfigValidChannel()){
		ExitProgram();
		MWCaptureExitInstance();
		return 1;
	}

	if(!OpenChannel(argc, argv)){
		ExitProgram();
		MWCaptureExitInstance();
		return 1;
	}

	ReadWriteEDID(argc, argv);

	CloseDevice();

	ExitProgram();

	return 0;
}

//show the basic information of our capture card
void ShowInfo()
{
	BYTE byMaj, byMin;
	WORD wBuild;
	MWGetVersion(&byMaj, &byMin, &wBuild);

	printf("Magewell MWCapture SDK %d.%d.1.%d - ReadWriteEDID\n", byMaj, byMin, wBuild);
	printf("All Devices are supported.\n");
	printf("Usage:\n");
	printf("<EDID file full path> : \"X:\\X\\XX.bin\"\n");
	printf("Read EDID: ReadWriteEDID.exe <channel index>\n");
	printf("Write EDID: ReadWriteEDID.exe <channel index> <EDID file full path>\n");
	printf("Pro and Eco capture serial devices: Read EDID: ReadWriteEDID.exe <board id>:<channel id>\t\n");
	printf("Pro and Eco capture serial devices: Write EDID: ReadWriteEDID.exe <board id>:<channel id> <EDID file full path>\t\n");
}

void Initialize()
{
	hChannel = NULL;
	nProDevChannel = NULL;
	nChannelCount=0;

	MWCaptureInitInstance();
}

//test the permission of write file at the specified path
BOOL WriteFilePermissionTest()
{
	FILE* pFile = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	BOOL bRet = FALSE;
	bRet = GetPath(path,256);
	if(bRet!=TRUE)
		return FALSE;
	_mkdir(path);

	sprintf_s(path, "%s\\temp.bin", path);
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&pFile, wPath, _T("wb"));
	if (NULL == pFile)
	{
		if (errNum == 13){
			char szError[256]={0};
			size_t tSZ = 256;
			strerror_s(szError,tSZ,errNum);
			printf("\nError: %s\n", szError);
		}

		printf("Please restart the application with administrator permission.\n");

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}

		return FALSE;
	}
	else
	{
		fclose(pFile);
		pFile = NULL;
		remove(path);

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}

		return TRUE;
	}
}

//find the amount of valid capture card and storing it in the var "nProDevChannel"
bool ConfigValidChannel()
{
	nChannelCount = MWGetChannelCount();
	if (nChannelCount == 0) 
	{
		printf("ERROR: Can't find channels!\n");
		return false;
	}
	nProDevChannel = new int[nChannelCount];
	memset(nProDevChannel, -1, nChannelCount * sizeof(int));

	for (int i = 0; i < nChannelCount; i++)
	{
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (mr == MW_SUCCEEDED)
		{
			nProDevChannel[i] = i;
		}
	}

	if(nChannelCount < 2)
		printf("Find %d channel.\n", nChannelCount);
	else
		printf("Find %d channels.\n", nChannelCount);

	return true;
}

//open channel by default(open the 0 channel) or use command parameter to open the specified channel
BOOL OpenChannel(int argc, char* argv[])
{
	int t_n_dev_index=-1;
	for(int i=0;i<nChannelCount;i++){
		if(nProDevChannel[i]!=-1){
			t_n_dev_index=i;
			break;
		}
	}
	if(t_n_dev_index == -1)
	{
		printf("ERROR: Can't find any device!\n");
		return FALSE;
	}

	// Get <board id > <channel id> or <channel index>
	int byBoardId = -1;
	int byChannelId = -1;
	int nDevIndex = -1;
	BOOL bIndex = FALSE;

	MWCAP_CHANNEL_INFO videoInfo = { 0 };
	if (argc == 1) 
	{
		bIndex = TRUE;
		nDevIndex = t_n_dev_index;
	}
	else 
	{
		WCHAR *Argv1 = AnsiToUnicode(argv[1]);

		if (NULL == _tcsstr(Argv1, _T(":")))
		{
			bIndex = TRUE;

			if (wcslen((const wchar_t*)Argv1) > 2)
				nDevIndex = -1;
			else if (wcslen((const wchar_t*)Argv1) == 2)
			{
				if ((Argv1[0] >= '0' && Argv1[0] <= '9') && (Argv1[1] >= '0' && Argv1[1] <= '9'))
					nDevIndex = _tstoi(Argv1);
				else
					nDevIndex = -1;
			}
			else if (wcslen((const wchar_t*)Argv1) == 1)
				nDevIndex = (Argv1[0] >= '0' && Argv1[0] <= '9') ? _tstoi(Argv1) : -1;

			if (nDevIndex < 0 || nDevIndex >= MWGetChannelCount())
			{
				printf("\nERROR: Invalid params!\n");

				if(Argv1 != NULL)
				{
					delete[] Argv1;
					Argv1 = NULL;
				}

				return FALSE;
			}
		}
		else
		{
			bIndex = FALSE;

			if (wcslen((const wchar_t*)Argv1) == 3)
			{
				if (Argv1[0] >= '0' && Argv1[0] <= '9')
					byBoardId = Argv1[0] - '0';
				else if (Argv1[0] >= 'a' && Argv1[0] <= 'f')
					byBoardId = Argv1[0] - 'a' + 10;
				else if (Argv1[0] >= 'A' && Argv1[0] <= 'F')
					byBoardId = Argv1[0] - 'A' + 10;
				else
					byBoardId = -1;

				if ((Argv1[2] >= '0' && Argv1[2] <= '3'))
					byChannelId = _tstoi(&Argv1[2]);
				else
					byChannelId = -1;
			}
			else
			{
				byBoardId = -1;
				byChannelId = -1;
			}

			if (-1 == byBoardId || -1 == byChannelId)
			{
				printf("\nERROR: Invalid params!\n");

				if(Argv1 != NULL)
				{
					delete[] Argv1;
					Argv1 = NULL;
				}

				return FALSE;
			}
		}

		if(Argv1 != NULL)
		{
			delete[] Argv1;
			Argv1 = NULL;
		}
	}

	if (bIndex == TRUE)
	{
		WCHAR path[128] = {0};
		MWGetDevicePath(nProDevChannel[nDevIndex], path);
		hChannel = MWOpenChannelByPath(path);
		if (hChannel == NULL) {
			printf("ERROR: Open channel %d error!\n", nDevIndex);
			return FALSE;
		}
	}
	else
	{
		hChannel = MWOpenChannel(byBoardId, byChannelId);
		if (hChannel == NULL) {
			printf("ERROR: Open channel %X:%d error!\n", byBoardId, byChannelId);
			return FALSE;
		}
	}

	if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) 
	{
		printf("ERROR: Can't get channel info!\n");
		return FALSE;
	}

	printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n", videoInfo.byBoardIndex, videoInfo.byChannelIndex);
	printf("Product Name: %s\n", videoInfo.szProductName);
	printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);

	return TRUE;
}

void ReadWriteEDID(int argc, char* argv[])
{
	BOOL bWriteMode = FALSE;
	if (argc == 3) 
	{
		WCHAR *Argv2 = AnsiToUnicode(argv[2]);

		if (0 != (_taccess(Argv2, 4)) || FALSE == Getfilesuffix(Argv2)) 
		{
			printf("ERROR: unknown EDID file!\n");

			if(Argv2 != NULL)
			{
				delete[] Argv2;
				Argv2 = NULL;
			}

			return;
		}

		bWriteMode = TRUE;

		FILE* pFile = NULL;
		_tfopen_s(&pFile, Argv2, _T("rb"));
		if (pFile == NULL)
		{
			FileFailed(pFile);

			if(Argv2 != NULL)
			{
				delete[] Argv2;
				Argv2 = NULL;
			}

			return;
		}

		BYTE byData = 0;
		int nCount = fread(&byData, 1, 1, pFile);
		if (nCount != 1 || byData != 0x00)
		{
			FileFailed(pFile);

			if(Argv2 != NULL)
			{
				delete[] Argv2;
				Argv2 = NULL;
			}

			return;
		}

		for (int i = 0; i < 6; i++)
		{
			nCount = fread(&byData, 1, 1, pFile);
			if (nCount != 1 || byData != 0xff)
			{
				FileFailed(pFile);

				if(Argv2 != NULL)
				{
					delete[] Argv2;
					Argv2 = NULL;
				}

				return;
			}
		}

		nCount = fread(&byData, 1, 1, pFile);
		if (nCount != 1 || byData != 0x00)
		{
			FileFailed(pFile);

			if(Argv2 != NULL)
			{
				delete[] Argv2;
				Argv2 = NULL;
			}

			return;
		}
		
		fclose(pFile);
		pFile = NULL;

		if(Argv2 != NULL)
		{
			delete[] Argv2;
			Argv2 = NULL;
		}
	}

	DWORD dwVideoSource = 0;
	DWORD dwAudioSource = 0;
	if (MW_SUCCEEDED != MWGetVideoInputSource(hChannel, &dwVideoSource)) 
	{
		printf("ERROR: Can't get video input source!\n");
		return;
	}
	if (MW_SUCCEEDED != MWGetAudioInputSource(hChannel, &dwAudioSource)) 
	{
		printf("ERROR: Can't get audio input source!\n");
		return;
	}
	if (INPUT_TYPE(dwVideoSource) != MWCAP_VIDEO_INPUT_TYPE_HDMI || INPUT_TYPE(dwAudioSource) != MWCAP_AUDIO_INPUT_TYPE_HDMI) {
		printf("Type of input source is not HDMI !\n");
		return;
	}

	MW_RESULT xr;

	if (bWriteMode) 
	{
		WCHAR *Argv2 = AnsiToUnicode(argv[2]);
		FILE * pFile = NULL;
		_tfopen_s(&pFile, Argv2, _T("rb"));
		if (pFile != NULL) 
		{
			BYTE byData[1024];
			int nSize = (int)fread(byData, 1, 1024, pFile);
				
			xr = MWSetEDID(hChannel, byData, nSize);
			if (xr == MW_SUCCEEDED) 
			{
				printf("Set EDID succeeded!\n");
			}
			else 
			{
				printf("ERROR: Set EDID!\n");
			}

			fclose(pFile);
			pFile = NULL;
		}
		else 
		{
			printf("ERROR: Read EDID file!\n");
			printf("Please restart the application with administrator permission.\n");
		}

		if(Argv2 != NULL)
		{
			delete[] Argv2;
			Argv2 = NULL;
		}
	}
	else 
	{
		FILE * pFile = NULL;
		char path[256];
		WCHAR *wPath = NULL;
		BOOL bRet = FALSE;
		bRet = GetPath(path,256);
		if(bRet!=TRUE){
			printf("\nCan't get the save path.\n");
			return;
		}
		_mkdir(path);

		sprintf_s(path, "%s\\ReadWriteEDID.bin", path);
		wPath = AnsiToUnicode(path);

		printf("\nThe file is saved in %s.\n\n", path);

		_tfopen_s(&pFile, wPath, _T("wb"));
		if (pFile != NULL) 
		{
			ULONG ulSize = 256;
			BYTE byData[256];
			xr = MWGetEDID(hChannel, byData, &ulSize);
			if (xr == MW_SUCCEEDED) {
				int nWriteSize = (int)fwrite(byData, 1, 256, pFile);
				if (nWriteSize == ulSize) 
				{
					printf("Write EDID to ReadWriteEDID.bin OK!\n");
				}
				else 
				{
					printf("ERROR: Write ReadWriteEDID.bin!\n");
				}
			}
			else 
			{
				printf("ERROR: Get EDID Info!\n");
			}

			fclose(pFile);
			pFile = NULL;
		}
		else 
		{
			printf("ERROR: Open ReadWriteEDID.bin!\n");
			printf("Please restart the application with administrator permission.\n");
		}

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}
	}
}

BOOL Getfilesuffix(const TCHAR* pFilePath)
{
	wstring str((const wchar_t*)pFilePath);
	str = str.substr(str.find_last_of('\\') + 1);
	str = str.substr(str.find_first_of('.') + 1);
	
	if (0 != _wcsicmp(L"BIN", str.c_str()))
		return FALSE;

	return TRUE;
}

void FileFailed(FILE* pFile)
{
	fclose(pFile);
	pFile = NULL;

	printf("ERROR: unknown EDID file!\n");
	printf("\nPress ENTER to exit...\n");
	getchar();
}

//stop capture and free memory.
void CloseDevice()
{
	if (hChannel != NULL){
		MWCloseChannel(hChannel);
		hChannel=NULL;
	}
	if (nProDevChannel != NULL){
		delete[] nProDevChannel;
		nProDevChannel=NULL;
	}

	MWCaptureExitInstance();
}

void ExitProgram(){
	if (nProDevChannel != NULL){
		delete[] nProDevChannel;
		nProDevChannel=NULL;
	}

	fflush(stdin);
	printf("Press \"Enter\" key to exit...\n");
	getchar();
}

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

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException)  
{  
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
	dumpInfo.ExceptionPointers = pException;  
	dumpInfo.ThreadId = GetCurrentThreadId();  
	dumpInfo.ClientPointers = TRUE;  

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
	CloseHandle(hDumpFile);  
}


LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)  
{     
	CreateDumpFile(L"ReadWriteEDID.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
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