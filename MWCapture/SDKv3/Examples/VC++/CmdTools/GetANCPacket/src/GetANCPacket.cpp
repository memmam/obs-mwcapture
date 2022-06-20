/************************************************************************************************/
// GetANCPacket.cpp : Defines the entry point for the console application.

// MAGEWELL PROPRIETARY INFORMATION

// The following license only applies to head files and library within Magewell's SDK 
// and not to Magewell's SDK as a whole. 

// Copyrights © Nanjing Magewell Electronics Co., Ltd. ("Magewell") All rights reserved.

// Magewell grands to any person who obtains the copy of Magewell's head files and library 
// the rights,including without limitation, to use, modify, publish, sublicense, distribute
// the Software on the conditions that all the following terms are met:
// - The above copyright notice shall be retained in any circumstances.
// -The following disclaimer shall be included in the software and documentation and/or 
// other materials provided for the purpose of publish, distribution or sublicense.

// THE SOFTWARE IS PROVIDED BY MAGEWELL "AS IS" AND ANY EXPRESS, INCLUDING BUT NOT LIMITED TO,
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
#include <iostream>
#include "LibMWCapture\MWCapture.h"
#include "cmdline.h"
#include <direct.h>

using namespace std;

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

typedef struct _anc_param{
	BOOLEAN	m_b_vanc;
	BOOLEAN m_b_hanc;
	BYTE	m_by_did;
	BYTE	m_by_sdid;
}mw_anc_param_t;

HCHANNEL		hChannel;
HANDLE			hExitThreadEvent;
HANDLE			hGetANCThread;
HANDLE			hGetInputThread;
int				*nProDevChannel;
int				nChannelCount;
mw_anc_param_t	g_anc_param[4];
bool			g_anc_param_valid;

int byBoardId = -1;
int byChannelId = -1;
int nDevIndex = -1;
BOOL bIndex = FALSE;
int nPacketNum = 600;

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

bool	 ParseCmdLine(int argc, char* argv[]);
void	 Initialize();
BOOL	 WriteFilePermissionTest();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, char* argv[]);
void	 CloseDevice();
void	 ExitProgram();
wchar_t* AnsiToUnicode( const char* szStr );
char*	 UnicodeToAnsi( const wchar_t* szStr );
DWORD WINAPI GetANCPacketThread(LPVOID lpThreadParameter);
DWORD WINAPI GetInputThread(LPVOID lpThreadParameter);
BOOL	 GetPath(char* szPath,int nSize);

int main(int argc, char* argv[])
{
	SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	bool t_b_prase_cmdline = false;
	t_b_prase_cmdline = ParseCmdLine(argc,argv);
	if(!t_b_prase_cmdline)
		ExitProgram();

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
	
	hGetANCThread=NULL;
	hExitThreadEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	hGetANCThread=CreateThread(NULL,0,GetANCPacketThread,NULL,0,NULL);
	hGetInputThread=CreateThread(NULL,0,GetInputThread,NULL,0,NULL);

	HANDLE t_array_thread[]={hGetANCThread,hGetInputThread};
	DWORD t_dw_ret=WaitForMultipleObjects(2,t_array_thread,FALSE,INFINITE);
	if(t_dw_ret==WAIT_OBJECT_0){
		printf("Press \'Enter\' key to exit getting input thread.\n");
		WaitForSingleObject(hGetInputThread,INFINITE);
	}
	else if(t_dw_ret==WAIT_OBJECT_0+1){
		SetEvent(hExitThreadEvent);
		WaitForSingleObject(hGetANCThread,INFINITE);
	}

	CloseDevice();

	ExitProgram();

	return 0;
}

bool ParseCmdLine(int argc, char* argv[])
{
	BYTE byMaj, byMin;
	WORD wBuild;
	MWGetVersion(&byMaj, &byMin, &wBuild);

	printf("Magewell MWCapture SDK %d.%d.1.%d - GetANCPacket\n", byMaj, byMin, wBuild);
	printf("Devices with SDI interface are supported\n");
	printf("\n");

	cmdline::parser a;
	a.add<int>("id", '\0', "device index(examples:\"--id f\" value range: 0-32 default value:0)", false, 0,cmdline::range(0,32));
	a.add<int>("bid",'\0',"border index(examples:\"--bid f\" value range: 0-15(0xf) default value:0)",false,0,cmdline::range(0,15));
	a.add<int>("cid",'\0',"channel index(examples:\"--cid f\" value range: 0-8 default value:0)",false,0,cmdline::range(0,8));

	a.add<int>("anc0_is_vanc",'\0',"set ANC 0 VANC(examples:\"--anc0_is_vanc 0\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc0_is_hanc",'\0',"set ANC 0 HANC(examples:\"--anc0_is_hanc 1\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc0_did",'\0',"set ANC 0 DID(examples:\"--anc0_did 0x61\" value range: 1-255 default value:0x61)",false,0x61,cmdline::range(1,255));
	a.add<int>("anc0_sdid",'\0',"set ANC 0 SDID(examples:\"--anc0_sdid 0x01\" value range: 1-255 default value:0x01)",false,0x01,cmdline::range(1,255));

	a.add<int>("anc1_is_vanc",'\0',"set ANC 1 VANC(examples:\"--anc1_is_vanc 0\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc1_is_hanc",'\0',"set ANC 1 HANC(examples:\"--anc1_is_hanc 1\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc1_did",'\0',"set ANC 1 DID(examples:\"--anc1_did 0x61\" value range: 1-255 default value:0x61)",false,0x61,cmdline::range(1,255));
	a.add<int>("anc1_sdid",'\0',"set ANC 1 SDID(examples:\"--anc1_sdid 0x02\" value range: 1-255 default value:0x02)",false,0x02,cmdline::range(1,255));

	a.add<int>("anc2_is_vanc",'\0',"set ANC 1 VANC(examples:\"--anc2_is_vanc 0\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc2_is_hanc",'\0',"set ANC 1 HANC(examples:\"--anc2_is_hanc 1\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc2_did",'\0',"set ANC 1 DID(examples:\"--anc2_did 0x61\" value range: 1-255 default value:0x61)",false,0x61,cmdline::range(1,255));
	a.add<int>("anc2_sdid",'\0',"set ANC 1 SDID(examples:\"--anc2_sdid 0x01\" value range: 1-255 default value:0x01)",false,0x01,cmdline::range(1,255));

	a.add<int>("anc3_is_vanc",'\0',"set ANC 1 VANC(examples:\"--anc3_is_vanc 0\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc3_is_hanc",'\0',"set ANC 1 HANC(examples:\"--anc3_is_hanc 1\" value range: 0:false 1:true default value:0)",false,0,cmdline::oneof(0,1));
	a.add<int>("anc3_did",'\0',"set ANC 1 DID(examples:\"--anc3_did 0x61\" value range: 1-255 default value:0x61)",false,0x61,cmdline::range(1,255));
	a.add<int>("anc3_sdid",'\0',"set ANC 1 SDID(examples:\"--anc3_sdid 0x01\" value range: 1-255 default value:0x01)",false,0x01,cmdline::range(1,255));

	a.add<int>("packet_num",'\0',"set ANC Capture Packet num(examples:\"--packet_num 600\" value range: 1-65535 default value:600)",false,600,cmdline::range(1,65535));

	bool t_b_ret = a.parse_check(argc, argv);
	if(!t_b_ret){
		printf("Examples:\nGetANCPacket.exe --id 0 --anc0_is_vanc 1 --anc0_is_hanc 0 --anc0_did 0x61 --anc0_sdid 0x01 --packet_num 600\n");
	}

	if(a.exist("id"))
		nDevIndex = a.get<int>("id");
	else{
		if(a.exist("bid"))
			byBoardId = a.get<int>("bid");
		if(a.exist("cid"))
			byChannelId = a.get<int>("cid");
	}

	if(a.exist("packet_num"))
		nPacketNum = a.get<int>("packet_num");

	if(nDevIndex==-1){
		if(byBoardId==-1||byChannelId==-1){
			nDevIndex = -1;
			bIndex = true;
		}
		else
			bIndex = false;
	}
	else
		bIndex = true;
	
	g_anc_param_valid=false;
	for(int i=0;i<4;i++){
		char t_cs_is_vanc[16]={0};
		char t_cs_is_hanc[16]={0};
		char t_cs_did[16]={0};
		char t_cs_sdid[16]={0};
		sprintf_s(t_cs_is_vanc,16,"anc%d_is_vanc",i);
		sprintf_s(t_cs_is_hanc,16,"anc%d_is_hanc",i);
		sprintf_s(t_cs_did,16,"anc%d_did",i);
		sprintf_s(t_cs_sdid,16,"anc%d_sdid",i);

		g_anc_param[i].m_b_vanc = a.get<int>(t_cs_is_vanc);
		g_anc_param[i].m_b_hanc = a.get<int>(t_cs_is_hanc);
		g_anc_param[i].m_by_did = a.get<int>(t_cs_did);
		g_anc_param[i].m_by_sdid = a.get<int>(t_cs_sdid);

		if(g_anc_param[i].m_b_vanc==TRUE||
			g_anc_param[i].m_b_hanc==TRUE){
			g_anc_param_valid=true;
		}

	}

	if(g_anc_param_valid==false){
		g_anc_param[0].m_b_vanc = true;
		g_anc_param[0].m_b_hanc = true;
	}

	return t_b_ret;
}

void Initialize()
{
	hChannel = NULL;
	hExitThreadEvent=NULL;
	nProDevChannel = NULL;
	nChannelCount=0;

	MWCaptureInitInstance();
}

//test the permission of write file at the specified path
BOOL WriteFilePermissionTest()
{
	FILE* fp = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	BOOL bRet = FALSE;
	bRet = GetPath(path,256);
	if(bRet!=TRUE)
		return FALSE;
	_mkdir(path);
	sprintf_s(path, "%s\\anc_data.bin", path);
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&fp, wPath, _T("wb"));
	if (NULL == fp)
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
		fclose(fp);
		fp = NULL;
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

	//looking for devices with SDI interface
	for (int i = 0; i < nChannelCount; i++)
	{
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if(strstr(info.szProductName,"USB")!=NULL)
			continue;
		if(mr!=MW_SUCCEEDED) continue;
		WCHAR t_wcs_path[256]={0};
		mr=MWGetDevicePath(i,t_wcs_path);
		if(mr!=MW_SUCCEEDED) continue;
		HCHANNEL t_channel=MWOpenChannelByPath(t_wcs_path);
		DWORD t_dw_count=0;
		mr=MWGetVideoInputSourceArray(t_channel,NULL,&t_dw_count);
		if(mr!=MW_SUCCEEDED){
			MWCloseChannel(t_channel);
			continue;
		}
		//number of input source types is less than 6
		DWORD t_arr_source[16]={0};
		mr=MWGetVideoInputSourceArray(t_channel,t_arr_source,&t_dw_count);
		if(mr!=MW_SUCCEEDED){
			MWCloseChannel(t_channel);
			continue;
		}
		MWCloseChannel(t_channel);
		for(int j=0;j<t_dw_count;j++){
			if(INPUT_TYPE(t_arr_source[j])==MWCAP_VIDEO_INPUT_TYPE_SDI){
				nProDevChannel[i] = i;
			}
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
	int t_n_pro_index=-1;
	for(int i=0;i<nChannelCount;i++){
		if(nProDevChannel[i]!=-1){
			t_n_pro_index=nProDevChannel[i];
			break;
		}
	}

	if(t_n_pro_index == -1)
	{
		printf("ERROR: Can't find Devices with SDI interface!\n");
		return FALSE;
	}
	
	// Get <board id > <channel id> or <channel index>


	MWCAP_CHANNEL_INFO videoInfo = { 0 };
	if (nDevIndex == -1&&bIndex== TRUE) 
	{
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nProDevChannel[t_n_pro_index], &videoInfo)) 
		{
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}
		nDevIndex = t_n_pro_index;
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

//1.clear up previous ANC settings and packets
//2.configure the ANC settings.
//3.get the ANC information by calling MWCaptureGetSDIANCPacket function and write it as .bin file.
DWORD WINAPI GetANCPacketThread(LPVOID lpThreadParameter){
	//clear up pre anc settings
	//0-3, below 4 index
	for(int i=0;i<4;i++)
	{
		MWCaptureSetSDIANCType(hChannel,i,FALSE,FALSE,0x0,0x00);
	}

	if (hChannel != NULL) 
	{
		MW_RESULT ret = MW_SUCCEEDED;
		for(int i=0;i<4;i++)
			ret = MWCaptureSetSDIANCType(
			hChannel,
			i,
			g_anc_param[i].m_b_hanc,//true,
			g_anc_param[i].m_b_vanc,//false,
			g_anc_param[i].m_by_did,
			g_anc_param[i].m_by_sdid);

		// Clear old packets
		MWCAP_SDI_ANC_PACKET ancPacket = { 0 };
		do 
		{
			ret = MWCaptureGetSDIANCPacket(hChannel, &ancPacket);
		} 
		while (ret == MW_SUCCEEDED && ancPacket.byDID != 0);

		HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		HNOTIFY hNotify = MWRegisterNotify(hChannel, hNotifyEvent, MWCAP_NOTIFY_NEW_SDI_ANC_PACKET);

		FILE* fp = NULL;
		char path[256];
		WCHAR *wPath = NULL;
		BOOL bRet = FALSE;
		bRet = GetPath(path,256);
		if(bRet!=TRUE)
			return FALSE;

		_mkdir(path);

		sprintf_s(path, "%s\\anc_data.bin", path);
		wPath = AnsiToUnicode(path);

		printf("\nThe file is saved in %s.\n\n", path);

		errno_t errNum = _tfopen_s(&fp, wPath, _T("wb"));

		HANDLE t_arr_event[]={hNotifyEvent,hExitThreadEvent};

		int t_n_index = 0;
		while(t_n_index<nPacketNum){
			DWORD t_dw_ret=WaitForMultipleObjects(2,t_arr_event,FALSE,INFINITE);
			if(t_dw_ret!=WAIT_OBJECT_0)
				break;

			ULONGLONG ullStatusBits;
			ret = MWGetNotifyStatus(hChannel, hNotify, &ullStatusBits);

			if (ullStatusBits & MWCAP_NOTIFY_NEW_SDI_ANC_PACKET) {
				do {
					ret = MWCaptureGetSDIANCPacket(hChannel, &ancPacket);
					if (ret == MW_SUCCEEDED && ancPacket.byDID != 0) {
						fwrite(&ancPacket, ancPacket.byDC + 3, 1, fp);
						_tprintf(_T("GetSDIANCPacket[%d], DID[0x%02X], SDID[0x%02X], Length[%d]\n"), t_n_index++, ancPacket.byDID, ancPacket.bySDID, ancPacket.byDC);
					}
				} while (ret == MW_SUCCEEDED && ancPacket.byDID != 0);
			}
		}


		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}

		fclose(fp);
		MWUnregisterNotify(hChannel, hNotify);
		CloseHandle(hNotifyEvent);
	}

	return 0;
}

DWORD WINAPI GetInputThread(LPVOID lpThreadParameter){
	printf("Press \'Enter\' key to exit getting ANC.\n");
	char ch;
	while (ch=cin.get()){
		if(ch=='\n'){
			break;
		}
	}
	return 0;
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

	exit(0);
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
	CreateDumpFile(L"GetANCPacket.dmp",pException);  
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