/************************************************************************************************/
// main.cpp : Defines the entry point for the console application.

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

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <direct.h>
#include "capturethread.h"
#include "sample.h"
#include "filewriter.h"

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

HCHANNEL hChannel;
int		 *nDevIndexArray;
int		 nProDevCount;
int		 nChannelCount;
TCHAR	 outFile[100];

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void	 ShowInfo();
void	 Initialize();
BOOL	 WriteFilePermissionTest();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, _TCHAR* argv[]);
void	 PrintUsage();
char	 *GetSignalStrColor(DWORD dwColorFormat);
void	 CloseDevice();
void	 OutMessage();
void	 ExitProgram();
wchar_t* AnsiToUnicode( const char* szStr );
char*	 UnicodeToAnsi( const wchar_t* szStr );


int _tmain(int argc,TCHAR *argv[])
{
	ShowInfo();

	Initialize();

	if(!WriteFilePermissionTest()){
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

	CloseDevice();

	ExitProgram();

	return 0;
}

void ShowInfo()
{
	BYTE byMaj, byMin;
	WORD wBuild;
	MWGetVersion(&byMaj, &byMin, &wBuild);
	
	printf("Magewell MWCapture SDK %d.%d.1.%d - NvidiaCodec\n", byMaj, byMin, wBuild);
	printf("USB and Eco Devices are not supported\n");
	printf("Usage:\n");
	printf("NvidiaCodec.exe <channel index>\n");
	printf("Pro capture serial devices: NvidiaCodec.exe <board id>:<channel id>\t\n");
}

void Initialize()
{
	hChannel = NULL;
	nDevIndexArray=NULL;
	nChannelCount=0;

	MWCaptureInitInstance();
}

BOOL WriteFilePermissionTest()
{
	FILE* pFile = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	strcpy_s(path, getenv("HOMEDRIVE"));
	strcat_s(path, getenv("HOMEPATH"));
	sprintf_s(path, "%s\\temp.h264", path);
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&pFile, wPath, _T("wb"));
	if (NULL == pFile)
	{
		if (errNum == 13)
			printf("\nError: %s\n", strerror(errNum));

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

bool ConfigValidChannel()
{
	nChannelCount = MWGetChannelCount();
	if(nChannelCount>0){
		nDevIndexArray=new int[nChannelCount];
		memset(nDevIndexArray,-1,nChannelCount*sizeof(int));
	}
	nProDevCount = 0;
	for (int i = 0; i < nChannelCount; i++)
	{
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (strcmp(info.szFamilyName, "Pro Capture") == 0){
			nProDevCount ++;
			nDevIndexArray[i]=i;
		}
	}

	if (nProDevCount <= 0)
	{
		printf("\nERROR: Can't find channels!\n");
		return false;
	}

	return true;
}

BOOL OpenChannel(int argc, _TCHAR* argv[])
{
	//Params parse
	BYTE nBoardID = 0;
	BYTE nChannelID = 0;
	int nFrameNumber = 100;
	BOOL bPushInfo = FALSE;
	BOOL bSetChannel = FALSE;

	BOOL bIndex = FALSE;
	int nDevIndex = -1;

	char path[256];
	WCHAR *wPath = NULL;
	strcpy_s(path, getenv("HOMEDRIVE"));
	strcat_s(path, getenv("HOMEPATH"));
	strcat_s(path, "\\Magewell");
#if(_MSC_VER>=1800)
	_mkdir(path);
#else
	mkdir(path);
#endif
	strcat_s(path, "\\output.h264");
	wPath = AnsiToUnicode(path);

	memset(outFile, 0, sizeof(outFile));	
	_tcscpy(outFile, wPath);
	for (int i = 1; i < argc; i++)
	{
		if (_tcsicmp(argv[i], L"-i") == 0)
		{
			i++;
			if (NULL == _tcsstr(argv[i], _T(":")))
			{
				if (wcslen(argv[i]) > 2)
					nDevIndex = -1;
				else if (wcslen(argv[i]) == 2)
				{
					if ((argv[i][0] >= '0' && argv[i][0] <= '9') && (argv[i][1] >= '0' && argv[i][1] <= '9'))
						nDevIndex = _tstoi(argv[1]);
					else
						nDevIndex = -1;
				}
				else if (wcslen(argv[i]) == 1)
					nDevIndex = (argv[i][0] >= '0' && argv[i][0] <= '9') ? _tstoi(argv[i]) : -1;

				if (nDevIndex < 0 || nDevIndex >= nProDevCount)
				{
					printf("\nERROR: Invalid params!\n");

					if(wPath != NULL)
					{
						delete[] wPath;
						wPath = NULL;
					}

					return FALSE;
				}
				
				bIndex = TRUE;
			}
			else
			{
				if (wcslen(argv[i]) == 3)
				{
					if (argv[i][0] >= '0' && argv[i][0] <= '9')
						nBoardID = argv[i][0] - '0';
					else if(argv[i][0] >= 'a' && argv[i][0] <= 'f')
						nBoardID = argv[i][0] - 'a' + 10;
					else if(argv[i][0] >= 'A' && argv[i][0] <= 'F')
						nBoardID = argv[i][0] - 'A' + 10;
					else
						nBoardID = -1;

					if ((argv[i][2] >= '0' && argv[i][2] <= '3'))
						nChannelID = _tstoi(&argv[i][2]);
					else
						nChannelID = -1;
				}
				else
				{
					nBoardID = -1;
					nChannelID = -1;
				}

				if (-1 == nBoardID || -1 == nChannelID)
				{
					printf("\nERROR: Invalid params!\n");

					if(wPath != NULL)
					{
						delete[] wPath;
						wPath = NULL;
					}

					return FALSE;
				}

				bIndex = FALSE;
			}
			bSetChannel = TRUE;
		}
		else if (_tcsicmp(argv[i], L"-n") == 0)
		{
			nFrameNumber = _tstoi(argv[++i]);
		}
		else if (_tcsicmp(argv[i], L"-o") == 0)
		{
			i++;
			_tcscpy(outFile, argv[i]);
		}
		else if (_tcsicmp(argv[i], L"-d") == 0)
		{
			bPushInfo = TRUE;
		}
		else
		{
			PrintUsage();
			printf("Invalid params,please check!\n");

			if(wPath != NULL)
			{
				delete[] wPath;
				wPath = NULL;
			}

			return FALSE;
		}
	}

	if(wPath != NULL)
	{
		delete[] wPath;
		wPath = NULL;
	}

	if(nProDevCount < 2)
		printf("Find %d channel.\n", nProDevCount);
	else
		printf("Find %d channels.\n", nProDevCount);

	int t_n_dev_index=-1;
	if (!bSetChannel)
	{
		for(int i=0;i<nChannelCount;i++){
			if(nDevIndexArray[i]!=-1){
				t_n_dev_index=i;
				break;
			}
		}
		if(t_n_dev_index==-1){
			printf("ERROR: No valid channel!\n");
			return FALSE;
		}
		MWCAP_CHANNEL_INFO videoInfo = { 0 };
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(t_n_dev_index, &videoInfo)) {
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}

		bIndex = FALSE;
		nDevIndex = -1;
		nBoardID = videoInfo.byBoardIndex;
		nChannelID = videoInfo.byChannelIndex;



	}

	//Engine run
	InputParams params = { 0 };
	CCaptureThread capture;
	CSample sample;
	CFileWriter writer;

	// Open channel
	HCHANNEL hChannel = NULL;
	if (bIndex == TRUE)
	{
		WCHAR path[128] = {0};
		MWGetDevicePath(nDevIndex, path);
		hChannel = MWOpenChannelByPath(path);
		if (hChannel == NULL) {
			printf("ERROR: Open Channel error! Please check specified %d !\n", nDevIndex);
			return FALSE;
		}
	}
	else
	{
		hChannel = MWOpenChannel(nBoardID, nChannelID);
		if (hChannel == NULL) {
			printf("ERROR: Open Channel error! Please check specified %d:%d !\n",nBoardID,nChannelID);
			return FALSE;
		}


	}

	MWCAP_CHANNEL_INFO videoInfo = { 0 };
	if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) {
		printf("ERROR: Can't get channel info!\n");
		return FALSE;
	}
	printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n", videoInfo.byBoardIndex, videoInfo.byChannelIndex);
	printf("Product Name: %s\n", videoInfo.szProductName);
	printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);

	MWCAP_VIDEO_SIGNAL_STATUS vStatus;
	if (MW_SUCCEEDED != MWGetVideoSignalStatus(hChannel, &vStatus))
	{
		printf("Get Video Signal Status error!\n");
		MWCloseChannel(hChannel);
		return FALSE;
	}
	if (vStatus.state != MWCAP_VIDEO_SIGNAL_LOCKED)
	{
		printf("No input signal!\n");
		MWCloseChannel(hChannel);
		return FALSE;
	}
	else
	{
		params.bitrate = 5000000;
		params.fps = 10000000.0 / vStatus.dwFrameDuration;
		params.width = vStatus.cx;
		params.height = vStatus.cy;
		params.numB = 0;
		params.framesToEncode = nFrameNumber;
		params.presetGUID = NV_ENC_PRESET_HP_GUID;
		params.bPushInfo = bPushInfo;

		printf("Input Signal:\n");
		printf("	width: %d\n", params.width);
		printf("	height: %d\n", params.height);
		printf("	fps: %.2f\n", params.fps);
		printf("	color format: %s\n\n", GetSignalStrColor(vStatus.colorFormat));
		printf("Encode %d frames , color format NV12 , fps %.2f:\n", params.framesToEncode, params.fps);
	}
	MWCloseChannel(hChannel);

	if (!writer.OpenFile(outFile))
	{
		printf("Open outfile error!\n");
		printf("Please restart the application with administrator permission.\n");
		return FALSE;
	}

	if (!sample.InitEncode(&writer))
	{
		printf("Init encode error! Please check GPU support!\n");
		return FALSE;
	}
	if (!sample.CreateEncode(&params))
	{
		printf("Create encode error!\n");
		return FALSE;
	}

	if (!capture.Create(nBoardID, nChannelID, &sample))
	{
		printf("Create capture error!\n");
		return FALSE;
	}

	while(!sample.QueryEncodeEndStatus());

	capture.Destroy();
	sample.FlushEncoder();
	printf("\n");
	capture.GetCaptureEndStatus();
	sample.QueryProcessTime();
	sample.Exit();
	writer.CloseFile();

	return TRUE;
}

void PrintUsage()
{
	printf("NVDIA HW-Encoding Example.\n");
	printf("Encoding captured video data into h264 format.\n");
	printf("USB Devices are not supported\n");
	printf("Usage:\n");
	printf("	NvidiaCodec.exe -i <boardID>:<channelID> / [<channel index>] -n frameNumber -d -o outfile\n");
	printf("	'-i' you can also use <channel index> to open the device\n");
	printf("	If '-i' is not specified,default channel 0 will be used.\n");
	printf("	If '-d' is specified, the debug info will be printed.\n\n");
}

char * GetSignalStrColor(DWORD dwColorFormat)
{
	switch (dwColorFormat)
	{
	case MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN:
		return "FORMAT_UNKNOWN";
	case MWCAP_VIDEO_COLOR_FORMAT_RGB:
		return "FORMAT_RGB";
	case MWCAP_VIDEO_COLOR_FORMAT_YUV601:
		return "FORMAT_YUV601";
	case MWCAP_VIDEO_COLOR_FORMAT_YUV709:
		return "FORMAT_YUV709";
	case MWCAP_VIDEO_COLOR_FORMAT_YUV2020:
		return "FORMAT_YUV2020";
	case MWCAP_VIDEO_COLOR_FORMAT_YUV2020C:
		return "FORMAT_YUV2020C";
	}

	return "FORMAT_UNKNOWN";
}

void OutMessage()
{
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
	if(nDevIndexArray!=NULL){
		delete nDevIndexArray;
		nDevIndexArray=NULL;
	}

	MWCaptureExitInstance();
}

void ExitProgram(){
	if(nDevIndexArray!=NULL){
		delete nDevIndexArray;
		nDevIndexArray=NULL;
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
	CreateDumpFile(L"NvidiaCodec.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}