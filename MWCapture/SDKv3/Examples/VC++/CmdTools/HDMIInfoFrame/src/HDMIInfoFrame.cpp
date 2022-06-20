/************************************************************************************************/
// HDMIInfoFrame.cpp : Defines the entry point for the console application.

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

// for dump info
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

#include "LibMWCapture\MWCapture.h"

HCHANNEL hChannel;
int *nProDevChannel;
int nChannelCount;

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException);
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void ShowInfo();
void Initialize();
bool ConfigValidChannel();
BOOL OpenChannel(int argc, char *argv[]);
void PrintHDMIInfo();
void CloseDevice();
void ExitProgram();
wchar_t *AnsiToUnicode(const char *szStr);
char *UnicodeToAnsi(const wchar_t *szStr);

int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter(
		(LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	ShowInfo();

	Initialize();

	if (!ConfigValidChannel()) {
		ExitProgram();
		MWCaptureExitInstance();
		return 1;
	}

	if (!OpenChannel(argc, argv)) {
		ExitProgram();
		MWCaptureExitInstance();
		return 1;
	}

	PrintHDMIInfo();

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

	printf("Magewell MWCapture SDK %d.%d.1.%d - HDMIInfoFrame\n", byMaj,
	       byMin, wBuild);
	printf("Devices with HDMI interface are supported.\n");
	printf("Usage:\n");
	printf("HDMIInfoFrame.exe <channel index>\n");
	printf("Pro and Eco capture serial devices: HDMIInfoFrame.exe <board id>:<channel id>\t\n");
}

void Initialize()
{
	hChannel = NULL;
	nProDevChannel = NULL;
	nChannelCount = 0;

	MWCaptureInitInstance();
}

//find the amount of valid capture card and storing it in the var "nProDevChannel"
bool ConfigValidChannel()
{
	nChannelCount = MWGetChannelCount();
	if (nChannelCount == 0) {
		printf("ERROR: Can't find channels!\n");
		return false;
	}
	nProDevChannel = new int[nChannelCount];
	memset(nProDevChannel, -1, nChannelCount * sizeof(int));

	//looking for devices with HDMI interface
	for (int i = 0; i < nChannelCount; i++) {
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (mr != MW_SUCCEEDED)
			continue;
		WCHAR t_wcs_path[256] = {0};
		mr = MWGetDevicePath(i, t_wcs_path);
		if (mr != MW_SUCCEEDED)
			continue;
		HCHANNEL t_channel = MWOpenChannelByPath(t_wcs_path);
		DWORD t_dw_count = 0;
		mr = MWGetVideoInputSourceArray(t_channel, NULL, &t_dw_count);
		if (mr != MW_SUCCEEDED) {
			MWCloseChannel(t_channel);
			continue;
		}
		//number of input source types is less than 6
		DWORD t_arr_source[16] = {0};
		mr = MWGetVideoInputSourceArray(t_channel, t_arr_source,
						&t_dw_count);
		if (mr != MW_SUCCEEDED) {
			MWCloseChannel(t_channel);
			continue;
		}
		MWCloseChannel(t_channel);
		for (int j = 0; j < t_dw_count; j++) {
			if (INPUT_TYPE(t_arr_source[j]) ==
			    MWCAP_VIDEO_INPUT_TYPE_HDMI) {
				nProDevChannel[i] = i;
			}
		}
	}

	if (nChannelCount < 2)
		printf("Find %d channel.\n", nChannelCount);
	else
		printf("Find %d channels.\n", nChannelCount);

	return true;
}

//open channel by default(open the 0 channel) or use command parameter to open the specified channel
BOOL OpenChannel(int argc, char *argv[])
{
	int t_n_dev_index = -1;
	for (int i = 0; i < nChannelCount; i++) {
		if (nProDevChannel[i] != -1) {
			t_n_dev_index = nProDevChannel[i];
			break;
		}
	}
	if (t_n_dev_index == -1) {
		printf("ERROR: Can't find Devices with HDMI interface!\n");
		return FALSE;
	}

	// Get <board id > <channel id> or <channel index>
	int byBoardId = -1;
	int byChannelId = -1;
	int nDevIndex = -1;
	BOOL bIndex = FALSE;

	MWCAP_CHANNEL_INFO videoInfo = {0};
	if (argc == 1) {
		if (MW_SUCCEEDED !=
		    MWGetChannelInfoByIndex(nProDevChannel[t_n_dev_index],
					    &videoInfo)) {
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}

		bIndex = TRUE;
		nDevIndex = t_n_dev_index;
	} else if (argc > 2) {
		printf("ERROR：Too many params!\n");
		return FALSE;
	} else {
		WCHAR *Argv1 = AnsiToUnicode(argv[1]);
		if (NULL == _tcsstr(Argv1, _T(":"))) {
			bIndex = TRUE;

			if (wcslen((const wchar_t *)Argv1) > 2)
				nDevIndex = -1;
			else if (wcslen((const wchar_t *)Argv1) == 2) {
				if ((Argv1[0] >= '0' && Argv1[0] <= '9') &&
				    (Argv1[1] >= '0' && Argv1[1] <= '9'))
					nDevIndex = _tstoi(Argv1);
				else
					nDevIndex = -1;
			} else if (wcslen((const wchar_t *)Argv1) == 1)
				nDevIndex =
					(argv[1][0] >= '0' && argv[1][0] <= '9')
						? _tstoi(Argv1)
						: -1;

			if (nDevIndex < 0 || nDevIndex >= MWGetChannelCount()) {
				printf("\nERROR: Invalid params!\n");

				if (Argv1 != NULL) {
					delete[] Argv1;
					Argv1 = NULL;
				}

				return FALSE;
			}
		} else {
			bIndex = FALSE;

			if (wcslen((const wchar_t *)Argv1) == 3) {
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
			} else {
				byBoardId = -1;
				byChannelId = -1;
			}

			if (-1 == byBoardId || -1 == byChannelId) {
				printf("\nERROR: Invalid params!\n");

				if (Argv1 != NULL) {
					delete[] Argv1;
					Argv1 = NULL;
				}

				return FALSE;
			}
		}

		if (Argv1 != NULL) {
			delete[] Argv1;
			Argv1 = NULL;
		}
	}

	if (bIndex == TRUE) {
		WCHAR path[128] = {0};
		MWGetDevicePath(nProDevChannel[nDevIndex], path);
		hChannel = MWOpenChannelByPath(path);
		if (hChannel == NULL) {
			printf("ERROR: Open channel %d error!\n", nDevIndex);
			return FALSE;
		}
	} else {
		hChannel = MWOpenChannel(byBoardId, byChannelId);
		if (hChannel == NULL) {
			printf("ERROR: Open channel %X:%d error!\n", byBoardId,
			       byChannelId);
			return FALSE;
		}
	}

	if (MW_SUCCEEDED != MWGetChannelInfo(hChannel, &videoInfo)) {
		printf("ERROR: Can't get channel info!\n");
		return FALSE;
	}

	printf("Open channel - BoardIndex = %X, ChannelIndex = %d.\n",
	       videoInfo.byBoardIndex, videoInfo.byChannelIndex);
	printf("Product Name: %s\n", videoInfo.szProductName);
	printf("Board SerialNo: %s\n\n", videoInfo.szBoardSerialNo);

	return TRUE;
}

void PrintHDMIInfo()
{
	MW_RESULT xr;
	MWCAP_INPUT_SPECIFIC_STATUS status;
	xr = MWGetInputSpecificStatus(hChannel, &status);
	if (xr == MW_SUCCEEDED) {
		if (!status.bValid) {
			printf("ERROR: Input signal is invalid!\n");
		} else if (status.dwVideoInputType != 1) {
			printf("ERROR: Input signal is not HDMI!\n");
		} else {
			DWORD dwValidFlag = 0;
			xr = MWGetHDMIInfoFrameValidFlag(hChannel,
							 &dwValidFlag);
			if (xr == MW_SUCCEEDED) {
				if (dwValidFlag == 0) {
					printf("No HDMI InfoFrame!\n");
				} else {
					HDMI_INFOFRAME_PACKET packet;
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_AVI) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_AVI,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame AVI OK!\n");

							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_AUDIO) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_AUDIO,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame Audio OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_SPD) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_SPD,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame SPD OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_MS) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_MS,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame MS OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_VS) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_VS,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame VS OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_ACP) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_ACP,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame ACP OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_ISRC1) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_ISRC1,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame ISRC1 OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_ISRC2) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_ISRC2,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame ISRC2 OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_GAMUT) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_GAMUT,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame GAMUT OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_VBI) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_VBI,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame VBI OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
					if (dwValidFlag &
					    MWCAP_HDMI_INFOFRAME_MASK_HDR) {
						xr = MWGetHDMIInfoFramePacket(
							hChannel,
							MWCAP_HDMI_INFOFRAME_ID_HDR,
							&packet);
						if (xr == MW_SUCCEEDED) {
							printf("Get HDMI InfoFrame HDR OK!\n");
							printf("Length = %d, Buffer is ",
							       packet.header
								       .byLength);
							for (int i = 0;
							     i <
							     packet.header
								     .byLength;
							     i++) {
								printf("%02x ",
								       packet.abyPayload
									       [i]);
							}
							printf("\n\n");
						}
					}
				}
			} else {
				printf("ERROR: Get HDMI InfoFrame Flag!\n");
			}
		}
	} else {
		printf("ERROR: Get Specific Status error!\n");
	}
}

//stop capture and free memory.
void CloseDevice()
{
	if (hChannel != NULL) {
		MWCloseChannel(hChannel);
		hChannel = NULL;
	}
	if (nProDevChannel != NULL) {
		delete[] nProDevChannel;
		nProDevChannel = NULL;
	}

	MWCaptureExitInstance();
}

void ExitProgram()
{
	if (nProDevChannel != NULL) {
		delete[] nProDevChannel;
		nProDevChannel = NULL;
	}

	fflush(stdin);
	printf("Press \"Enter\" key to exit...\n");
	getchar();
}

//A2W
inline wchar_t *AnsiToUnicode(const char *szStr)
{
	int nLen =
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, NULL, 0);
	if (nLen == 0) {
		return NULL;
	}
	wchar_t *pResult = new wchar_t[nLen];
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szStr, -1, pResult, nLen);
	return pResult;
}

// W2A
inline char *UnicodeToAnsi(const wchar_t *szStr)
{
	int nLen =
		WideCharToMultiByte(CP_ACP, 0, szStr, -1, NULL, 0, NULL, NULL);
	if (nLen == 0) {
		return NULL;
	}
	char *pResult = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szStr, -1, pResult, nLen, NULL, NULL);
	return pResult;
}

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException)
{
	HANDLE hDumpFile = CreateFile(lpstrDumpFilePathName, GENERIC_WRITE, 0,
				      NULL, CREATE_ALWAYS,
				      FILE_ATTRIBUTE_NORMAL, NULL);

	MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
	dumpInfo.ExceptionPointers = pException;
	dumpInfo.ThreadId = GetCurrentThreadId();
	dumpInfo.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
			  MiniDumpNormal, &dumpInfo, NULL, NULL);
	CloseHandle(hDumpFile);
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)
{
	CreateDumpFile(L"HDMIInfoFrame.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}