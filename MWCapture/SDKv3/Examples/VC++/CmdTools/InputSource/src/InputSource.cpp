/************************************************************************************************/
// InputSource.cpp : Defines the entry point for the console application.

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

#include <windows.h>
#include <conio.h>

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

#include "LibMWCapture\MWCapture.h"


HCHANNEL	hChannel;
int			*nProDevChannel;
int			nChannelCount; 
BOOLEAN		g_bScan;
BOOLEAN		g_bAVLink;
DWORD		g_dwVideoInputCount;
DWORD		g_dwAudioInputCount;
DWORD		g_dwVideoInput;
DWORD		g_dwAudioInput;
DWORD		g_dwArrayVideoInput[32];
DWORD		g_dwArrayAudioInput[32];

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void	 ShowInfo();
void	 Initialize();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, char* argv[]);
void	 PrintInputSourceInfo();
bool	 GetInputYN();
int		 GetInputSelect(int max);
void	 SetInputSourceConfig();
void	 GetVideoInputName(DWORD dwVideoInput, char* pszName, int nSize);
void	 GetAudioInputName(DWORD dwAudioInput, char* pszName, int nSize);
void	 CloseDevice();
void	 ExitProgram();
wchar_t* AnsiToUnicode( const char* szStr );
char*	 UnicodeToAnsi( const wchar_t* szStr );


int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler); 

	ShowInfo();

	Initialize();

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

	PrintInputSourceInfo();

	SetInputSourceConfig();

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

	printf("Magewell MWCapture SDK %d.%d.1.%d - InputSource\n", byMaj, byMin, wBuild);
	printf("All Devices are supported.\n");
	printf("Usage:\n");
	printf("InputSource.exe <channel index>\n");
	printf("Pro and Eco capture serial devices: InputSource.exe <board id>:<channel id>\t\n");
}

void Initialize()
{
	hChannel = NULL;
	nProDevChannel = NULL;
	nChannelCount=0;

	MWCaptureInitInstance();
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
		if (mr==MW_SUCCEEDED)
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
			t_n_dev_index=nProDevChannel[i];
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
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nProDevChannel[t_n_dev_index], &videoInfo)) 
		{
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}

		bIndex = TRUE;
		nDevIndex = t_n_dev_index;
	}
	else if (argc > 2)
	{
		printf("ERROR：Too many params!\n");
		return FALSE;
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
				nDevIndex = (argv[1][0] >= '0' && argv[1][0] <= '9') ? _tstoi(Argv1) : -1;

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

	if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) {
		printf("ERROR: Can't get channel info!\n");
		return FALSE;
	}

	printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n", videoInfo.byBoardIndex, videoInfo.byChannelIndex);
	printf("Product Name: %s\n", videoInfo.szProductName);
	printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);

	return TRUE;
}

void PrintInputSourceInfo()
{
	MW_RESULT xr;

	//Input Source Scan
	g_bScan=FALSE;
	xr = MWGetInputSourceScan(hChannel,&g_bScan);
	if(xr!=MW_SUCCEEDED){
		printf("Get input source scan failed.\nPlease re-run the program later.\n");
		return;
	}
	else{
		printf("Input Source Auto Scan:");
		if(g_bScan==FALSE)
			printf("FALSE\n");
		else
			printf("TRUE\n");
	}


	// Video Input Source
	g_dwVideoInput = 0;
	xr = MWGetVideoInputSource(hChannel, &g_dwVideoInput);

	g_dwVideoInputCount = 0;
	xr = MWGetVideoInputSourceArray(hChannel, NULL, &g_dwVideoInputCount);
	if (xr == MW_SUCCEEDED && g_dwVideoInputCount > 0) 
	{
		printf("Video Input Count : %d\n", g_dwVideoInputCount);

		memset(g_dwArrayVideoInput,-1,sizeof(DWORD)*32);
		xr = MWGetVideoInputSourceArray(hChannel, g_dwArrayVideoInput, &g_dwVideoInputCount);
		if (xr == MW_SUCCEEDED) 
		{
			char szInputName[16] = { 0 };
			for (DWORD i = 0; i < g_dwVideoInputCount; i++) 
			{
				GetVideoInputName(g_dwArrayVideoInput[i], szInputName, 16);
				if(g_dwVideoInput==g_dwArrayVideoInput[i])
					printf("* [%d] %s\n", i, szInputName);
				else
					printf("  [%d] %s\n", i, szInputName);
			}
		}
	}

	if(g_dwVideoInput!=0){
		char szInputName[16] = { 0 };
		GetVideoInputName(g_dwVideoInput, szInputName, 16);
		printf("Current Video Input Source: %s\n", szInputName);
	}
	printf("\n");

	// AV Link
	g_bAVLink=FALSE;
	xr=MWGetAVInputSourceLink(hChannel,&g_bAVLink);
	if(xr!=MW_SUCCEEDED){
		printf("Get AVLink failed.\nPlease re-run the program later.\n");
		return;
	}
	else{
		printf("AV Input Source Link:");
		if(g_bAVLink==FALSE)
			printf("FALSE\n");
		else
			printf("TRUE\n");
	}

	// Audio Input Source
	g_dwAudioInput = 0;
	xr = MWGetAudioInputSource(hChannel, &g_dwAudioInput);

	g_dwAudioInputCount = 0;
	xr = MWGetAudioInputSourceArray(hChannel, NULL, &g_dwAudioInputCount);
	if (xr == MW_SUCCEEDED && g_dwAudioInputCount > 0) 
	{
		printf("Audio Input Count : %d\n", g_dwAudioInputCount);

		memset(g_dwArrayAudioInput,-1,sizeof(DWORD)*32);
		xr = MWGetAudioInputSourceArray(hChannel, g_dwArrayAudioInput, &g_dwAudioInputCount);
		if (xr == MW_SUCCEEDED) 
		{
			char szInputName[16] = { 0 };
			for (DWORD i = 0; i < g_dwAudioInputCount; i++) 
			{
				GetAudioInputName(g_dwArrayAudioInput[i], szInputName, 16);
				if(g_dwAudioInput==g_dwArrayAudioInput[i])
					printf("* [%d] %s\n", i, szInputName);
				else
					printf("  [%d] %s\n", i, szInputName);
			}
		}
	}

	if (g_dwAudioInput!=0) 
	{
		char szInputName[16] = { 0 };
		GetAudioInputName(g_dwAudioInput, szInputName, 16);
		printf("Current Audio Input Source: %s\n", szInputName);
	}
	//if(g_bAVLink){
	//	printf("Audio Input Source will change with Video Input Source.\n\n");
	//}
	printf("\n");
}

bool GetInputYN(){
	bool bRet=false;

	char t_char='N';
	while(true){
		t_char=_getch();
		char t_cY='Y';
		char t_cN='N';
		if(t_char==t_cY||t_char==t_cN){
			printf("%c\n",t_char);
			break;
		}
		else{
			printf("Please input Y/N(uppercase letter)\n");
		}
	}

	bRet=(t_char=='Y')?true:false;

	return bRet;
}

int	GetInputSelect(int max){
	int t_n_ret=-1;

	char t_char='0';
	int t_n_real_fig=0;
	int t_n_figures=1;
	int t_n_10=0;
	int t_n_01=max%10;
	int	t_n_val=0;
	if(max/10>0){
		t_n_figures++;
		t_n_10=max/10;
	}

	while(true){
		t_char=_getch();
		if(t_n_real_fig>0)
			if(t_char==13){
				printf("\n");
				break;
			}				

		if(t_char>='0'&&t_char<='9'){
			if(t_n_real_fig==0){
				int t_n_var=t_char-'0';
				if(t_n_var>max)
					continue;
				else{
					t_n_val=t_n_var;
					printf("%c",t_char);
					t_n_real_fig++;
				}
			}
			else{
				int t_n_var=t_char-'0';
				t_n_var=t_n_var+t_n_val*10;
				if(t_n_var>max)
					continue;
				else{
					t_n_val=t_n_var;
					printf("%c",t_char);
					t_n_real_fig++;
				}
			}
		}
	}

	t_n_ret=t_n_val;
	return t_n_ret;
}

void SetInputSourceConfig()
{
	MW_RESULT xr=MW_SUCCEEDED;
	printf("Would like to change the Input Source Settings?Y/N\n\n");
	bool bY=GetInputYN();
	if(!bY)
		return;

	if(g_bScan)
		printf("Would you like to turn off InputSource Auto Scan? Y/N\n");
	else
		printf("Would you like to turn on InputSource Auto Scan? Y/N\n");
	
	bY=GetInputYN();

	if(bY){
		g_bScan=!g_bScan;
		xr=MWSetInputSourceScan(hChannel,g_bScan);
	}

	if(!g_bScan){
		printf("Would you like to set Video Input Source? Y/N\n");
		bY=GetInputYN();
		if(bY){
			printf("You can select from the following items.\n");
			char szInputName[16] = { 0 };
			for (DWORD i = 0; i < g_dwVideoInputCount; i++) {
				GetVideoInputName(g_dwArrayVideoInput[i], szInputName, 16);
				printf("	[%d] %s\n", i, szInputName);
			}
			if(g_dwVideoInputCount==1)
				printf("The default selection is [%d] %s\n",0,szInputName);
			else{
				printf("You can select from [%d] to [%d],which selection do you select? (%d-%d)",0,g_dwVideoInputCount-1,0,g_dwVideoInputCount-1);
				int t_n_sel=GetInputSelect(g_dwVideoInputCount-1);
				xr=MWSetVideoInputSource(hChannel,g_dwArrayVideoInput[t_n_sel]);
				if(xr==MW_SUCCEEDED){
					printf("Set Video Input Source Successfully.\n");
				}
				else{
					printf("Set Video Input Source Failed.\n");
				}
			}
		}
	}
	printf("\n");

	if(g_bAVLink)
		printf("Would you like to turn off AVLink? Y/N\n");
	else
		printf("Would you like to turn on AVLink? Y/N\n");

	bY=GetInputYN();

	if(bY){
		g_bAVLink=!g_bAVLink;
		xr=MWSetAVInputSourceLink(hChannel,g_bAVLink);
	}

	if(!g_bAVLink){
		printf("\n");

		printf("Would you like to set Audio Input Source? Y/N\n");
		bY=GetInputYN();
		if(bY){
			printf("You can select from the following items.\n");
			char szInputName[16] = { 0 };
			for (DWORD i = 0; i < g_dwAudioInputCount; i++) {
				GetAudioInputName(g_dwArrayAudioInput[i], szInputName, 16);
				printf("	[%d] %s\n", i, szInputName);
			}
			if(g_dwVideoInputCount==1)
				printf("The default selection is [%d] %s\n",0,szInputName);
			else{
				printf("You can select from [%d] to [%d],which selection do you select? (%d-%d)",0,g_dwAudioInputCount-1,0,g_dwAudioInputCount-1);
				int t_n_sel=GetInputSelect(g_dwAudioInputCount-1);
				xr=MWSetAudioInputSource(hChannel,g_dwArrayAudioInput[t_n_sel]);
				if(xr==MW_SUCCEEDED){
					printf("Set Audio Input Source Successfully.\n");
				}
				else{
					printf("Set Audio Input Source Failed.\n");
				}
			}
		}
	}

	printf("\n");

	printf("The current setting is:\n");
	PrintInputSourceInfo();
}

void GetVideoInputName(DWORD dwVideoInput, char* pszName, int nSize)
{
	switch (INPUT_TYPE(dwVideoInput)) 
	{
	case MWCAP_VIDEO_INPUT_TYPE_NONE:
		strcpy_s(pszName, nSize, "None");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_HDMI:
		strcpy_s(pszName, nSize, "HDMI");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_VGA:
		strcpy_s(pszName, nSize, "VGA");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_SDI:
		strcpy_s(pszName, nSize, "SDI");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_COMPONENT:
		strcpy_s(pszName, nSize, "Component");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_CVBS:
		strcpy_s(pszName, nSize, "CVBS");
		break;
	case MWCAP_VIDEO_INPUT_TYPE_YC:
		strcpy_s(pszName, nSize, "YC");
		break;
	}
}

void GetAudioInputName(DWORD dwAudioInput, char* pszName, int nSize)
{
	switch (INPUT_TYPE(dwAudioInput)) 
	{
	case MWCAP_AUDIO_INPUT_TYPE_NONE:
		strcpy_s(pszName, nSize, "None");
		break;
	case MWCAP_AUDIO_INPUT_TYPE_HDMI:
		strcpy_s(pszName, nSize, "HDMI");
		break;
	case MWCAP_AUDIO_INPUT_TYPE_SDI:
		strcpy_s(pszName, nSize, "SDI");
		break;
	case MWCAP_AUDIO_INPUT_TYPE_LINE_IN:
		strcpy_s(pszName, nSize, "Line In");
		break;
	case MWCAP_AUDIO_INPUT_TYPE_MIC_IN:
		strcpy_s(pszName, nSize, "Mic In");
		break;
	}
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
	CreateDumpFile(L"InputSource.dmp",pException);  
	return EXCEPTION_EXECUTE_HANDLER;  
}