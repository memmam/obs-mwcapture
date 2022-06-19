/************************************************************************************************/
// AudioCapture.cpp : Defines the entry point for the console application.

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
#include <string.h>
#include <windows.h>
#include <direct.h>

// for dump info
#include <DbgHelp.h>
#pragma comment(lib,"DbgHelp.lib")

#include "WaveFile.h"

#include "LibMWCapture\MWCapture.h"

#define MAX_BIT_DEPTH_IN_BYTE (sizeof(DWORD))


HCHANNEL					hChannel;
int							*nProDevChannel;
MW_RESULT					xr;
MWCAP_AUDIO_SIGNAL_STATUS	audioSignalStatus;
int							nChannelCount;

void	 CreateDumpFile(LPCWSTR lpstrDumpFilePathName, EXCEPTION_POINTERS *pException);
LONG	 ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void     ShowInfo();
void	 Initialize();
BOOL	 WriteFilePermissionTest();
bool	 ConfigValidChannel();
BOOL	 OpenChannel(int argc, char* argv[]);
BOOL	 CheckSignal();
void	 StartCapture();
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

	if(!CheckSignal()){
		CloseDevice();
		ExitProgram();
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

	printf("Magewell MWCapture SDK %d.%d.1.%d - AudioCapture\n", byMaj, byMin, wBuild);
	printf("USB Devices are not supported\n");
	printf("Usage:\n");
	printf("AudioCapture.exe <channel index>\n");
	printf("AudioCapture.exe <board id>:<channel id>\t\n\n");	
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
	FILE *wavFile = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	BOOL bRet = FALSE;
	bRet = GetPath(path,256);
	if(bRet!=TRUE)
		return FALSE;
	_mkdir(path);
	sprintf_s(path, "%s\\temp.wav", path);
#ifdef _UNICODE
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&wavFile, wPath, _T("wb"));
#else
	wPath = AnsiToUnicode(path);
	errno_t errNum = _wfopen_s(&wavFile, wPath, L"wb");
#endif // _UNICODE

    
	if (NULL == wavFile)
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
		fclose(wavFile);
		wavFile = NULL;
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

	int nProCount = 0;
	for (int i = 0; i < nChannelCount; i++)
	{
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (strcmp(info.szFamilyName, "Pro Capture") == 0 ||
			strcmp(info.szFamilyName, "Eco Capture") == 0)
		{
			nProDevChannel[i] = i;
			nProCount++;
		}
	}
	nChannelCount = nProCount;

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
		printf("ERROR: Can't find Pro Capture!\n");
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
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nProDevChannel[t_n_pro_index], &videoInfo))
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

//check if there is a signal source input
BOOL CheckSignal()
{
	DWORD dwInputCount = 0;
	xr = MWGetAudioInputSourceArray(hChannel, NULL, &dwInputCount);
	if (dwInputCount == 0) 
	{
		printf("ERROR: Can't find audio input!\n");
		return FALSE;
	}
	else
	{
		MWGetAudioSignalStatus(hChannel, &audioSignalStatus);
		if (audioSignalStatus.wChannelValid  == 0)
		{
			printf("ERROR: Audio signal is invalid\n");
			return FALSE;
		}
		else
		{
			int nSampleRate = audioSignalStatus.dwSampleRate;
			int nBitDepthInByte = audioSignalStatus.cBitsPerSample / 8;
			BOOL bIsLPCM = audioSignalStatus.bLPCM;

			char chSupChannels[128] = {0};
			for (int i = 0; i < 4; i++)
			{
				if (audioSignalStatus.wChannelValid & (0x01 << i))
				{
					sprintf_s(chSupChannels,128,"%s %d&%d;", chSupChannels, (i * 2 + 1), (i * 2 + 2));
				}
			}

			printf("Audio Signal: bValid = %d, SampleRate = %d, ChannelValid = %s\n",
					audioSignalStatus.bChannelStatusValid, audioSignalStatus.dwSampleRate, chSupChannels);

			return TRUE;
		}
	}
}

//start capture:
//1.calling the start function —— MWStartAudioCapture(hChannel);
//2.create event and register notify;
//3.wait for event notify(signal change or frame buffered) by MWGetNotifyStatus function and deal with it;
//4.when accept the frame buffered notification, you should capture the data and do what you want;

//Tips : 1)in our example, we add the var(llBegin and llEnd) to record the elapse time in capture, but this is no necessary.
//		 2)we capture the LPCM and save it as .wav, you can capture the audio data and do others. Also, the input signal may not be LPCM,
//         you should deal with it by the audio transmission standard.
//       3)the wav is Little-Endian, but other may be Big-Endian. it's different in saving data between the two mode.
void StartCapture()
{
	xr = MWStartAudioCapture(hChannel);
	if (xr != MW_SUCCEEDED) 
	{
		printf("ERROR: Open Audio Capture error!\n");
		return;
	}
	else
	{
		HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		HNOTIFY hNotify = MWRegisterNotify(hChannel, hEvent, MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE | MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED);

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(hChannel, &llBegin);

		int nSampleRate = audioSignalStatus.dwSampleRate;
		int nBitDepthInByte = audioSignalStatus.cBitsPerSample / 8;
		if(audioSignalStatus.cBitsPerSample%8!=0)
			nBitDepthInByte++;
		BOOL bIsLPCM = audioSignalStatus.bLPCM;

		int nSupChannelCount = 0;
		char chSupChannels[128] = {0};
		for (int i = 0; i < 4; i++)
		{
			if (audioSignalStatus.wChannelValid & (0x01 << i))
			{
				nSupChannelCount += 2;
				sprintf_s(chSupChannels,128,"%s %d&%d;", chSupChannels, (i * 2 + 1), (i * 2 + 2));
			}
		}

		CWaveFile file;
		char path[256];
		WCHAR *wPath = NULL;
		BOOL bRet = FALSE;
		bRet = GetPath(path,256);
		if(bRet!=TRUE)
			return;
		_mkdir(path);
		sprintf_s(path, "%s\\AudioCapture.wav", path);
		printf("\nThe file is saved in %s.\n\n", path);
		wPath = AnsiToUnicode(path);
        file.Init(wPath, nSampleRate, nSupChannelCount, nBitDepthInByte * 8);

		printf("Begin capture 1000 frames...\n");
		for (int i = 0; i < 1000; i++) 
		{
			WaitForSingleObject(hEvent, INFINITE);

			ULONGLONG ullStatusBits = 0LL;
			MWGetNotifyStatus(hChannel, hNotify, &ullStatusBits);

			if (ullStatusBits & MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE) 
			{
				MWGetAudioSignalStatus(hChannel, &audioSignalStatus);

				if (audioSignalStatus.wChannelValid  !=0)
				{
					memset(chSupChannels, 0, sizeof(chSupChannels));
					for (int i = 0; i < 4; i++)
					{
						if (audioSignalStatus.wChannelValid & (0x01 << i))
							sprintf_s(chSupChannels, 128,"%d&%d;", (i * 2 + 1), (i * 2 + 2));
					}

					printf("Audio Signal Changed: bValid = %d, SampleRate = %d, ChannelValid = 0x%x\n",
						audioSignalStatus.bChannelStatusValid, audioSignalStatus.dwSampleRate, audioSignalStatus.wChannelValid);
				}
				else
					printf("ERROR: Audio signal is invalid !\n");
							
				break;
			}

			if (ullStatusBits & MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED) 
			{
				do 
				{
					MWCAP_AUDIO_CAPTURE_FRAME audioFrame;
					xr = MWCaptureAudioFrame(hChannel, &audioFrame);

					if (xr == MW_SUCCEEDED && file.IsOpen() && bIsLPCM) 
					{
						BYTE* pbAudioFrame = (BYTE*)audioFrame.adwSamples;
						BYTE asAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * MAX_BIT_DEPTH_IN_BYTE];
									
						for (int j = 0; j < nSupChannelCount / 2; ++j)
						{
							for (int i = 0; i < MWCAP_AUDIO_SAMPLES_PER_FRAME; i++) 
							{
								int nWritePos	= (i * nSupChannelCount + j * 2) * nBitDepthInByte;
								int nReadPos	= (i * MWCAP_AUDIO_MAX_NUM_CHANNELS + j) * MAX_BIT_DEPTH_IN_BYTE;
								int nReadPos2	= (i * MWCAP_AUDIO_MAX_NUM_CHANNELS + j + MWCAP_AUDIO_MAX_NUM_CHANNELS / 2) * MAX_BIT_DEPTH_IN_BYTE;

								for (int k = 0; k < nBitDepthInByte; ++k) 
								{
									asAudioSamples[nWritePos + k]					= pbAudioFrame[nReadPos + MAX_BIT_DEPTH_IN_BYTE - nBitDepthInByte + k]; 
									asAudioSamples[nWritePos + nBitDepthInByte + k]	= pbAudioFrame[nReadPos2 + MAX_BIT_DEPTH_IN_BYTE - nBitDepthInByte + k];
								}
							}
						}
									
						file.Write((const BYTE*)asAudioSamples, MWCAP_AUDIO_SAMPLES_PER_FRAME * nSupChannelCount * nBitDepthInByte);
					}
				} while (xr == MW_SUCCEEDED);
			}
		}
		file.Exit();

		LONGLONG llEnd = 0LL;
		xr = MWGetDeviceTime(hChannel, &llEnd);

		printf("End capture, samples are %d, the duration is %d ms. \n", MWCAP_AUDIO_SAMPLES_PER_FRAME * 1000, (llEnd - llBegin) / 10000);

		printf("Write audio samples to AudioCapture.wav .\n");

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}

		MWUnregisterNotify(hChannel, hNotify);
		CloseHandle(hEvent);
	}

	MWStopAudioCapture(hChannel);
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
	CreateDumpFile(L"AudioCapture.dmp",pException);  
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