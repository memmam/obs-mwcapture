/************************************************************************************************/
// CaptureByTimer.cpp : Defines the entry point for the console application.

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

#include "CaptureByTimerPro.h"
#include "CaptureByTimerEco.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

#define TIMER_DURATION	400000LL	// 25 fps


HCHANNEL					hChannel;
int							*nPCIEDevChannel;
MW_RESULT					xr;
ULONG_PTR					gdiplusToken;
int							nChannelCount;

// Get <board id > <channel id> or <channel index>
int byBoardId = -1;
int byChannelId = -1;
int nDevIndex = -1;
BOOL bIndex = FALSE;

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void	 ShowInfo();
void	 Initialize();
BOOL	 WriteFilePermissionTest();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, char* argv[]);
BOOL	 ParseCmdLine(int argc, char* argv[]);
void	 StartCapture();
int		 GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
void     CloseDevice();
void	 ExitProgram();

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler); 

	ShowInfo();

	Initialize();

	if(!WriteFilePermissionTest()){
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	if(!ConfigValidChannel()){
		ExitProgram();
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	if(!OpenChannel(argc, argv)){
		ExitProgram();
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	StartCapture();

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

	printf("Magewell MWCapture SDK %d.%d.1.%d - CaptureByTimer\n", byMaj, byMin, wBuild);
	printf("USB Devices are not supported\n");
	printf("Usage:\n");
	printf("CaptureByTimer.exe <channel index>\n");
	printf("CaptureByTimer.exe <board id>:<channel id>\t\n\n");
}

void Initialize()
{
	hChannel = NULL;
	nPCIEDevChannel = NULL;
	nChannelCount=0;

	//initizlize Gdi+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	MWCaptureInitInstance();
}

//test the permission of write file at the specified path
BOOL WriteFilePermissionTest()
{
	FILE* bmpFile = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	BOOL bRet = FALSE;
	bRet = GetPath(path,256);
	if(bRet!=TRUE)
		return FALSE;
	_mkdir(path);

	sprintf_s(path, "%s\\temp.bmp", path);
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&bmpFile, wPath, _T("wb"));
	if (NULL == bmpFile)
	{
		if (errNum == 13){
			char szError[256]={0};
			size_t tSZ = 256;
			strerror_s(szError,tSZ,errNum);
			printf("\nError: %s\n", szError);
		}

		printf("Please restart the application with administrator permission.\n");
		printf("\nPress ENTER to exit...\n");
		getchar();

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}

		return FALSE;
	}
	else
	{
		fclose(bmpFile);
		bmpFile = NULL;
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
	nPCIEDevChannel = new int[nChannelCount];
	memset(nPCIEDevChannel, -1, nChannelCount * sizeof(int));

	int nPCIECount = 0;
	for (int i = 0; i < nChannelCount; i++)
	{
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (info.wFamilyID!=MW_FAMILY_ID_USB_CAPTURE){
			nPCIEDevChannel[i] = i;
			nPCIECount++;
		}
	}
	nChannelCount = nPCIECount;

	if(nChannelCount < 2)
		printf("Find %d channel.\n", nPCIECount);
	else
		printf("Find %d channels.\n", nPCIECount);

	return true;
}

//open channel by default(open the 0 channel) or use command parameter to open the specified channel
BOOL OpenChannel(int argc, char* argv[])
{
	int t_n_pro_index=-1;
	for(int i=0;i<nChannelCount;i++){
		if(nPCIEDevChannel[i]!=-1){
			t_n_pro_index=nPCIEDevChannel[i];
			break;
		}
	}
	if(t_n_pro_index == -1)
	{
		printf("ERROR: Can't find Pro Capture!\n");
		return FALSE;
	}

	MWCAP_CHANNEL_INFO videoInfo = { 0 };
	if (argc == 1) 
	{
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nPCIEDevChannel[t_n_pro_index], &videoInfo)) 
		{
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}

		bIndex = TRUE;
		nDevIndex = t_n_pro_index;
	}
	else if (argc > 2)
	{
		printf("ERROR：Too many params!\n");
		return FALSE;
	}
	else 
	{
		BOOL t_b_ret = false;
		t_b_ret = ParseCmdLine(argc,argv);
		if(!t_b_ret)
			return t_b_ret;
	}

	if (bIndex == TRUE)
	{
		WCHAR path[128] = {0};
		MWGetDevicePath(nPCIEDevChannel[nDevIndex], path);
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

	if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) {
		printf("ERROR: Can't get channel info!\n");
		return FALSE;
	}

	printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n", videoInfo.byBoardIndex, videoInfo.byChannelIndex);
	printf("Product Name: %s\n", videoInfo.szProductName);
	printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);
	return TRUE;
}

BOOL ParseCmdLine(int argc, char* argv[])
{
	BOOL t_b_ret = true;

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
			nDevIndex = (argv[1][0] >= '0' && argv[1][0] <= '9') ? _tstoi(Argv1) : -1;

		if (nDevIndex < 0 || nDevIndex >= nChannelCount)
		{
			printf("\nERROR: Invalid params!\n");

			if(Argv1 != NULL)
			{
				delete[] Argv1;
				Argv1 = NULL;
			}

			t_b_ret = FALSE;
			return t_b_ret;
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

			t_b_ret = FALSE;
			return t_b_ret;
		}
	}

	if(Argv1 != NULL)
	{
		delete[] Argv1;
		Argv1 = NULL;
	}

	return t_b_ret;
}

void StartCapture()
{
	if(hChannel!=NULL){
		MWCAP_CHANNEL_INFO t_channel_info;
		MWGetChannelInfo(hChannel,&t_channel_info);
		if(t_channel_info.wFamilyID == MW_FAMILY_ID_PRO_CAPTURE)
			CaptureByTimerPro(hChannel,TIMER_DURATION);
		else if(t_channel_info.wFamilyID == MW_FAMILY_ID_ECO_CAPTURE)
			CaptureByTimerEco(hChannel,TIMER_DURATION);
	}
}

//stop capture and free memory.
void CloseDevice()
{
	if (hChannel != NULL){
		MWCloseChannel(hChannel);
		hChannel=NULL;
	}
	if (nPCIEDevChannel != NULL){
		delete[] nPCIEDevChannel;
		nPCIEDevChannel=NULL;
	}

	MWCaptureExitInstance();
}

void ExitProgram(){
	if (nPCIEDevChannel != NULL){
		delete[] nPCIEDevChannel;
		nPCIEDevChannel=NULL;
	}

	fflush(stdin);
	printf("Press \"Enter\" key to exit...\n");
	getchar();
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
	CreateDumpFile(L"CaptureByTimer.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}