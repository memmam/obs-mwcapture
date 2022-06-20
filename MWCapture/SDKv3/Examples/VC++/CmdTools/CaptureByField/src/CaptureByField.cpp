/************************************************************************************************/
// CaptureByField.cpp : Defines the entry point for the console application.

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
#include <gdiplus.h>
#include <direct.h>

// for dump info
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

#include "LibMWCapture\MWCapture.h"

using namespace Gdiplus;

#define NUM_CAPTURE 100

HCHANNEL hChannel;
int *nProDevChannel;
MW_RESULT xr;
MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
ULONG_PTR gdiplusToken;
int nChannelCount;

void CreateDumpFile(LPCWSTR lpstrDumpFilePathName,
		    EXCEPTION_POINTERS *pException);
LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException);

void ShowInfo();
void Initialize();
BOOL WriteFilePermissionTest();
bool ConfigValidChannel();
BOOL OpenChannel(int argc, char *argv[]);
BOOL CheckSignal();
void StartCapture();
void CaptureIFrame();
int GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
void CloseDevice();
void ExitProgram();
wchar_t *AnsiToUnicode(const char *szStr);
char *UnicodeToAnsi(const wchar_t *szStr);
BOOL GetPath(char *szPath, int nSize);

int main(int argc, char *argv[])
{
	SetUnhandledExceptionFilter(
		(LPTOP_LEVEL_EXCEPTION_FILTER)ApplicationCrashHandler);

	ShowInfo();

	Initialize();

	if (!WriteFilePermissionTest()) {
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	if (!ConfigValidChannel()) {
		ExitProgram();
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	if (!OpenChannel(argc, argv)) {
		ExitProgram();
		GdiplusShutdown(gdiplusToken);
		MWCaptureExitInstance();
		return 1;
	}

	if (!CheckSignal()) {
		CloseDevice();
		ExitProgram();
		GdiplusShutdown(gdiplusToken);
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

	printf("Magewell MWCapture SDK %d.%d.1.%d - CaptureByField\n", byMaj,
	       byMin, wBuild);
	printf("USB and Eco Devices are not supported\n");
	printf("Usage:\n");
	printf("CaptureByField.exe <channel index>\n");
	printf("CaptureByField.exe <board id>:<channel id>\t\n\n");
}

void Initialize()
{
	hChannel = NULL;
	nProDevChannel = NULL;
	nChannelCount = 0;

	//initizlize Gdi+
	GdiplusStartupInput gdiplusStartupInput;

	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	MWCaptureInitInstance();
}

//test the permission of write file at the specified path
BOOL WriteFilePermissionTest()
{
	FILE *bmpFile = NULL;
	char path[256];
	WCHAR *wPath = NULL;
	BOOL bRet = FALSE;
	bRet = GetPath(path, 256);
	if (bRet != TRUE)
		return FALSE;
	_mkdir(path);
	sprintf_s(path, "%s\\temp.bmp", path);
	wPath = AnsiToUnicode(path);
	errno_t errNum = _tfopen_s(&bmpFile, wPath, _T("wb"));
	if (NULL == bmpFile) {
		if (errNum == 13) {
			char szError[256] = {0};
			size_t tSZ = 256;
			strerror_s(szError, tSZ, errNum);
			printf("\nError: %s\n", szError);
		}

		printf("Please restart the application with administrator permission.\n");
		printf("\nPress ENTER to exit...\n");
		getchar();

		if (wPath != NULL) {
			delete[] wPath;
			wPath = NULL;
		}

		return FALSE;
	} else {
		fclose(bmpFile);
		bmpFile = NULL;
		remove(path);

		if (wPath != NULL) {
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
	if (nChannelCount == 0) {
		printf("ERROR: Can't find channels!\n");
		return false;
	}
	nProDevChannel = new int[nChannelCount];
	memset(nProDevChannel, -1, nChannelCount * sizeof(int));

	int nProCount = 0;
	for (int i = 0; i < nChannelCount; i++) {
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (strcmp(info.szFamilyName, "Pro Capture") == 0) {
			nProDevChannel[i] = i;
			nProCount++;
		}
	}
	nChannelCount = nProCount;

	if (nChannelCount < 2)
		printf("Find %d channel.\n", nChannelCount);
	else
		printf("Find %d channels.\n", nChannelCount);

	return true;
}

//open channel by default(open the 0 channel) or use command parameter to open the specified channel
BOOL OpenChannel(int argc, char *argv[])
{
	int t_n_pro_index = -1;
	for (int i = 0; i < nChannelCount; i++) {
		if (nProDevChannel[i] != -1) {
			t_n_pro_index = nProDevChannel[i];
			break;
		}
	}
	if (t_n_pro_index == -1) {
		printf("ERROR: Can't find Pro Capture!\n");
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
		    MWGetChannelInfoByIndex(nProDevChannel[t_n_pro_index],
					    &videoInfo)) {
			printf("ERROR: Can't get channel info!\n");
			return FALSE;
		}

		bIndex = TRUE;
		nDevIndex = t_n_pro_index;
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

			if (nDevIndex < 0 || nDevIndex >= nChannelCount) {
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

//check the input status and related information of the video signal
BOOL CheckSignal()
{
	MWGetVideoSignalStatus(hChannel, &videoSignalStatus);

	switch (videoSignalStatus.state) {
	case MWCAP_VIDEO_SIGNAL_NONE:
		printf("Input signal status: NONE\n");
		break;
	case MWCAP_VIDEO_SIGNAL_UNSUPPORTED:
		printf("Input signal status: Unsupported\n");
		break;
	case MWCAP_VIDEO_SIGNAL_LOCKING:
		printf("Input signal status: Locking\n");
		break;
	case MWCAP_VIDEO_SIGNAL_LOCKED:
		printf("Input signal status: Locked\n");
		break;
	}

	if (videoSignalStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
		printf("Input signal resolution: %d x %d\n",
		       videoSignalStatus.cx, videoSignalStatus.cy);
		double fps =
			(videoSignalStatus.bInterlaced == TRUE)
				? (double)20000000LL /
					  videoSignalStatus.dwFrameDuration
				: (double)10000000LL /
					  videoSignalStatus.dwFrameDuration;
		printf("Input signal fps: %.2f\n", fps);
		printf("Input signal interlaced: %d\n",
		       videoSignalStatus.bInterlaced);
		printf("Input signal frame segmented: %d\n",
		       videoSignalStatus.bSegmentedFrame);
		if (videoSignalStatus.bInterlaced)
			return TRUE;
		else {
			printf("\nThis demo only supports interlaced signals.\n\n");
		}
	}
	return FALSE;
}

//start capture:
//We deal with video signals in different methods.
//Interlaced signal is captured by function  CaptureIFrame.It will capture all top fields and bottom fields.
//Progressive scanning (alternatively referred to as noninterlaced scanning) signal is captured by function
//CapturePFrame.It will capture the full frame.
void StartCapture()
{
	CaptureIFrame();
}

//CaptureIFrame:
//1.create notify event and capture event to receive capture messages;
//2.call the start function —— MWStartVideoCapture(hChannel, hCaptureEvent);
//3.register notify for notify event to receive notifies from capture card;
//4.wait for completed notify(MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED) and use the function MWCaptureVideoFrameToVirtualAddressEx to save data
//  ,WaitForSingleObject(hCaptureEvent, INFINITE) and hCaptureEvent means all data received ,then you can do what you want;
//5.call function MWGetVideoCaptureStatus to free memory source
//6.in our example, we save the video data as .bmp, this is not necessary, so we have to calling function(GetEncoderClsid) to get the CLSID and calling Bitmap::Save to save bitmap;
//7.close capture and unregister notify.

// Tips : in our example, we add the var(llTotalTime and llCurrent) to calculate the time in capturing, but this is not necessary.
void CaptureIFrame()
{
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	xr = MWStartVideoCapture(hChannel, hCaptureEvent);
	if (xr != MW_SUCCEEDED) {
		printf("ERROR: Open Video Capture error!\n");
	} else {
		MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
		MWGetVideoBufferInfo(hChannel, &videoBufferInfo);

		MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
		MWGetVideoFrameInfo(hChannel,
				    videoBufferInfo.iNewestBufferedFullFrame,
				    &videoFrameInfo);

		if (videoSignalStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
			// Allocate capture buffer
			int cx = videoSignalStatus.cx;
			int cy = videoSignalStatus.cy;

			Bitmap bitmap(cx, cy, PixelFormat24bppRGB);
			Rect rect(0, 0, cx, cy);
			BitmapData bitmapData;

			Status status = bitmap.LockBits(&rect,
							ImageLockModeWrite,
							PixelFormat24bppRGB,
							&bitmapData);

			BOOL bBottomUp = FALSE;
			if (bitmapData.Stride < 0) {
				bitmapData.Scan0 = ((LPBYTE)bitmapData.Scan0) +
						   bitmapData.Stride * (cy - 1);
				bitmapData.Stride = -bitmapData.Stride;
				bBottomUp = TRUE;
			}

			HNOTIFY hNotify = MWRegisterNotify(
				hChannel, hNotifyEvent,
				MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED);
			if (hNotify == NULL) {
				printf("ERROR: Register Notify error.\n");
			} else {
				double fps = (double)20000000LL /
					     videoSignalStatus.dwFrameDuration;
				printf("Begin to capture %d frames by %.2f fps...\n",
				       NUM_CAPTURE, fps);

				LONGLONG llTotalTime = 0LL;

				MWPinVideoBuffer(hChannel,
						 (LPBYTE)bitmapData.Scan0,
						 bitmapData.Stride * cy);
				for (int i = 0; i < NUM_CAPTURE; i++) {
					WaitForSingleObject(hNotifyEvent,
							    INFINITE);

					ULONGLONG ullStatusBits = 0;
					xr = MWGetNotifyStatus(hChannel,
							       hNotify,
							       &ullStatusBits);
					if (xr != MW_SUCCEEDED)
						continue;

					xr = MWGetVideoBufferInfo(
						hChannel, &videoBufferInfo);
					if (xr != MW_SUCCEEDED)
						continue;

					xr = MWGetVideoFrameInfo(
						hChannel,
						videoBufferInfo
							.iBufferedFieldIndex,
						&videoFrameInfo);
					if (xr != MW_SUCCEEDED)
						continue;

					if (ullStatusBits &
					    MWCAP_NOTIFY_VIDEO_FIELD_BUFFERED) {
						MWCAP_VIDEO_DEINTERLACE_MODE
						mode;
						if (videoBufferInfo
							    .iBufferedFieldIndex ==
						    0)
							mode = MWCAP_VIDEO_DEINTERLACE_TOP_FIELD;
						else
							mode = MWCAP_VIDEO_DEINTERLACE_BOTTOM_FIELD;

						xr = MWCaptureVideoFrameToVirtualAddressEx(
							hChannel,
							videoBufferInfo
								.iNewestBuffered,
							(LPBYTE)bitmapData.Scan0,
							bitmapData.Stride * cy,
							bitmapData.Stride,
							bBottomUp, NULL,
							MWFOURCC_BGR24, cx, cy,
							0,
							0,    //partical notify
							NULL, //OSD
							NULL, //OSD rect
							0, 100, 0, 100, 0,
							mode, //deinterlace mode
							MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
							NULL, NULL, 0, 0,
							MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
							MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
							MWCAP_VIDEO_SATURATION_UNKNOWN);
						WaitForSingleObject(
							hCaptureEvent,
							INFINITE);

						LONGLONG llCurrent = 0LL;
						xr = MWGetDeviceTime(
							hChannel, &llCurrent);

						MWCAP_VIDEO_CAPTURE_STATUS
						t_status;
						MWGetVideoCaptureStatus(
							hChannel, &t_status);
					}
				}
				MWUnpinVideoBuffer(hChannel,
						   (LPBYTE)bitmapData.Scan0);
				printf("End capture.\n");

				xr = MWUnregisterNotify(hChannel, hNotify);

				xr = MWStopVideoCapture(hChannel);

				bitmap.UnlockBits(&bitmapData);

				char path[256];
				WCHAR *wPath = NULL;
				BOOL bRet = FALSE;
				bRet = GetPath(path, 256);
				if (bRet != TRUE) {
					printf("\nCan't get the save path.\n");
					return;
				}
				_mkdir(path);
				sprintf_s(path, "%s\\CaptureByField.bmp", path);
				wPath = AnsiToUnicode(path);

				printf("\nThe last frame is saved in %s.\n\n",
				       path);

				CLSID pngClsid;
				GetEncoderClsid(
					(const wchar_t *)_T("image/bmp"),
					&pngClsid);
				bitmap.Save(wPath, &pngClsid, NULL);

				if (wPath != NULL) {
					delete[] wPath;
					wPath = NULL;
				}
			}
			CloseHandle(hNotifyEvent);
		}
		CloseHandle(hCaptureEvent);
	}

	MWStopVideoCapture(hChannel);

	GdiplusShutdown(gdiplusToken);
}

int GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;  // number of image encoders
	UINT size = 0; // size of the image encoder array in bytes

	ImageCodecInfo *pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1; // Failure

	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1; // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j; // Success
		}
	}

	free(pImageCodecInfo);
	return -1; // Failure
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
	CreateDumpFile(L"CaptureByField.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}

BOOL GetPath(char *szPath, int nSize)
{
	char szTmp[256];
	char *pszTmp = NULL;
	size_t tSZ = 0;
	if (_dupenv_s(&pszTmp, &tSZ, "HOMEDRIVE") == 0 && pszTmp != NULL) {
		strcpy_s(szTmp, pszTmp);
		free(pszTmp);
		pszTmp = NULL;
	} else
		return FALSE;

	if (_dupenv_s(&pszTmp, &tSZ, "HOMEPATH") == 0 && pszTmp != NULL) {
		strcat_s(szTmp, pszTmp);
		free(pszTmp);
		pszTmp = NULL;
	} else
		return FALSE;

	strcat_s(szTmp, "\\Magewell");
	if (nSize < 256)
		return FALSE;
	memcpy(szPath, szTmp, 256);

	return TRUE;
}