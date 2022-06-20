/************************************************************************************************/
// InputSignal.cpp : Defines the entry point for the console application.

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
void PrintInputSignalInfo();
void GetVideoColorName(MWCAP_VIDEO_COLOR_FORMAT color, char *pszName,
		       int nSize);
void GetVideoSDIType(SDI_TYPE type, char *pTypeName, int nSize);
void GetVideoScanFmt(SDI_SCANNING_FORMAT type, char *pFmtName, int nSize);
void GetVideoSamplingStruct(SDI_SAMPLING_STRUCT type, char *pStructName,
			    int nSize);
void GetVideoBitDepth(SDI_BIT_DEPTH type, char *pTypeName, int nSize);
void GetVideoSyncType(BYTE type, char *pTypeName, int nSize);
void GetVideoSDStandard(MWCAP_SD_VIDEO_STANDARD type, char *pTypeName,
			int nSize);
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

	PrintInputSignalInfo();

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

	printf("Magewell MWCapture SDK %d.%d.1.%d - InputSignal\n", byMaj,
	       byMin, wBuild);
	printf("All Devices are supported.\n");
	printf("Usage:\n");
	printf("InputSignal.exe <channel index>\n");
	printf("Pro and Eco capture serial devices: InputSignal.exe <board id>:<channel id>\t\n");
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

	for (int i = 0; i < nChannelCount; i++) {
		MWCAP_CHANNEL_INFO info;
		MW_RESULT mr = MWGetChannelInfoByIndex(i, &info);
		if (mr == MW_SUCCEEDED) {
			nProDevChannel[i] = i;
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
			t_n_dev_index = i;
			break;
		}
	}
	if (t_n_dev_index == -1) {
		printf("ERROR: Can't find any device!\n");
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

void PrintInputSignalInfo()
{
	MW_RESULT xr;

	// Specific Status
	MWCAP_INPUT_SPECIFIC_STATUS status;
	xr = MWGetInputSpecificStatus(hChannel, &status);
	if (xr == MW_SUCCEEDED) {
		printf("Input Specific Status: %s\n",
		       status.bValid == TRUE ? "Present" : "Not present");

		if (status.bValid) {
			switch (status.dwVideoInputType) {
			case MWCAP_VIDEO_INPUT_TYPE_NONE: {
				printf("---Type: NONE\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_HDMI: {
				printf("---Type: HDMI\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_VGA: {
				printf("---Type: VGA\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_SDI: {
				printf("---Type: SDI\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_COMPONENT: {
				printf("---Type: COMPONENT\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_CVBS: {
				printf("---Type: CVBS\n");
				break;
			}
			case MWCAP_VIDEO_INPUT_TYPE_YC: {
				printf("---Type: YC\n");
				break;
			}
			default:
				break;
			}

			if (status.dwVideoInputType ==
			    MWCAP_VIDEO_INPUT_TYPE_HDMI) {
				printf("---HDMI HDCP: %s\n",
				       status.hdmiStatus.bHDCP == TRUE ? "Yes"
								       : "No");
				printf("---HDMI HDMI Mode: %d\n",
				       status.hdmiStatus.bHDMIMode);
				printf("---HDMI Bit Depth: %d\n",
				       status.hdmiStatus.byBitDepth);
			} else if (status.dwVideoInputType ==
				   MWCAP_VIDEO_INPUT_TYPE_SDI) {
				char chType[32] = {0};
				GetVideoSDIType(status.sdiStatus.sdiType,
						chType, 32);

				char chScanfmt[32] = {0};
				GetVideoScanFmt(
					status.sdiStatus.sdiScanningFormat,
					chScanfmt, 32);

				char chSampleStruct[32] = {0};
				GetVideoSamplingStruct(
					status.sdiStatus.sdiSamplingStruct,
					chSampleStruct, 32);

				char chBitDepth[32] = {0};
				GetVideoBitDepth(status.sdiStatus.sdiBitDepth,
						 chBitDepth, 32);

				printf("---SDI Type: %s\n", chType);
				printf("---SDI Scanning Format: %s\n",
				       chScanfmt);
				printf("---SDI Bit Depth: %s\n", chBitDepth);
				printf("---SDI Sampling Struct: %s\n",
				       chSampleStruct);
			} else if (status.dwVideoInputType ==
				   MWCAP_VIDEO_INPUT_TYPE_VGA) {
				char chSyncType[32] = {0};
				GetVideoSyncType(status.vgaComponentStatus
							 .syncInfo.bySyncType,
						 chSyncType, 32);

				double dFrameDuration =
					(status.vgaComponentStatus.syncInfo
						 .bInterlaced == TRUE)
						? (double)20000000 /
							  status.vgaComponentStatus
								  .syncInfo
								  .dwFrameDuration
						: (double)10000000 /
							  status.vgaComponentStatus
								  .syncInfo
								  .dwFrameDuration;
				printf("---VGA SyncType: %s\n", chSyncType);
				printf("---VGA bHSPolarity: %d\n",
				       status.vgaComponentStatus.syncInfo
					       .bHSPolarity);
				printf("---VGA bVSPolarity: %d\n",
				       status.vgaComponentStatus.syncInfo
					       .bVSPolarity);
				printf("---VGA bInterlaced: %d\n",
				       status.vgaComponentStatus.syncInfo
					       .bInterlaced);
				printf("---VGA FrameDuration: %0.2f\n",
				       dFrameDuration);
			} else if (status.dwVideoInputType ==
				   MWCAP_VIDEO_INPUT_TYPE_CVBS) {
				char chSDStandard[32] = {0};
				GetVideoSDStandard(status.cvbsYcStatus.standard,
						   chSDStandard, 32);

				printf("---CVBS standard: %s\n", chSDStandard);
				printf("---CVBS b50Hz: %d\n",
				       status.cvbsYcStatus.b50Hz);
			}
		}
	}
	printf("\n");

	// Video Signal Status
	MWCAP_VIDEO_SIGNAL_STATUS vStatus;
	xr = MWGetVideoSignalStatus(hChannel, &vStatus);
	if (xr == MW_SUCCEEDED) {
		switch (vStatus.state) {
		case MWCAP_VIDEO_SIGNAL_LOCKED: {
			printf("Video Signal status: LOCKED\n");
			break;
		}
		case MWCAP_VIDEO_SIGNAL_LOCKING: {
			printf("Video Signal status: LOCKING\n");
			break;
		}
		case MWCAP_VIDEO_SIGNAL_UNSUPPORTED: {
			printf("Video Signal status: UNSUPPORTED\n");
			break;
		}
		case MWCAP_VIDEO_SIGNAL_NONE: {
			printf("Video Signal status: Not present\n");
			break;
		}
		default:
			break;
		}

		if (vStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
			char szColorName[16];
			GetVideoColorName(vStatus.colorFormat, szColorName, 16);

			double dFrameRate =
				(vStatus.bInterlaced == TRUE)
					? (double)20000000 /
						  vStatus.dwFrameDuration
					: (double)10000000 /
						  vStatus.dwFrameDuration;

			printf("Video Signal: \n");
			printf("---x, y: (%d, %d)\n", vStatus.x, vStatus.y);
			printf("---cx x cy: (%d x %d)\n", vStatus.cx,
			       vStatus.cy);
			printf("---cxTotal x cyTotal: (%d x %d)\n",
			       vStatus.cxTotal, vStatus.cyTotal);
			printf("---bInterlaced: %s\n",
			       vStatus.bInterlaced == TRUE ? "Yes" : "No");
			printf("---dwFrameRate: %0.2f\n", dFrameRate);
			printf("---nAspectX: %d\n", vStatus.nAspectX);
			printf("---nAspectY: %d\n", vStatus.nAspectY);
			printf("---bSegmentedFrame: %s\n",
			       vStatus.bSegmentedFrame == TRUE ? "Yes" : "No");
			printf("---colorFormat: %s\n", szColorName);

			switch (vStatus.satRange) {
			case MWCAP_VIDEO_SATURATION_UNKNOWN:
				printf("---saturationRange : %s\n",
				       "MWCAP_VIDEO_SATURATION_UNKNOWN");
				break;
			case MWCAP_VIDEO_SATURATION_FULL:
				printf("---saturationRange : %s\n",
				       "MWCAP_VIDEO_SATURATION_FULL");
				break;
			case MWCAP_VIDEO_SATURATION_LIMITED:
				printf("---saturationRange : %s\n",
				       "MWCAP_VIDEO_SATURATION_LIMITED");
				break;
			case MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT:
				printf("---saturationRange : %s\n",
				       "MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT");
				break;
			default:
				printf("---saturationRange : %s\n",
				       "MWCAP_VIDEO_SATURATION_UNKNOWN");
				break;
			}

			switch (vStatus.quantRange) {
			case MWCAP_VIDEO_QUANTIZATION_UNKNOWN:
				printf("---quantizationRange : %s\n",
				       "MWCAP_VIDEO_QUANTIZATION_UNKNOWN");
				break;
			case MWCAP_VIDEO_QUANTIZATION_FULL:
				printf("---quantizationRange : %s\n",
				       "MWCAP_VIDEO_QUANTIZATION_FULL");
				break;
			case MWCAP_VIDEO_QUANTIZATION_LIMITED:
				printf("---quantizationRange : %s\n",
				       "MWCAP_VIDEO_QUANTIZATION_LIMITED");
				break;
			default:
				printf("---quantizationRange : %s\n",
				       "MWCAP_VIDEO_QUANTIZATION_UNKNOWN");
				break;
			}
		}
	}

	printf("\n");

	// Audio Signal Status
	MWCAP_AUDIO_SIGNAL_STATUS aStatus;
	xr = MWGetAudioSignalStatus(hChannel, &aStatus);
	if (xr == MW_SUCCEEDED) {
		printf("Audio Signal Valid: %s\n",
		       aStatus.wChannelValid != 0 ? "Present" : "Not present");

		if (aStatus.wChannelValid != 0) {
			char chSupChannels[128] = {0};
			for (int i = 0; i < 4; i++) {
				if (aStatus.wChannelValid & (0x01 << i))
					sprintf_s(chSupChannels, 128,
						  "%s %d&%d;", chSupChannels,
						  (i * 2 + 1), (i * 2 + 2));
			}

			printf("---wChannelValid: %s\n", chSupChannels);
			printf("---bLPCM: %s\n",
			       aStatus.bLPCM == TRUE ? "Yes" : "No");
			printf("---cBitsPerSample: %d\n",
			       aStatus.cBitsPerSample);
			printf("---dwSampleRate: %d\n", aStatus.dwSampleRate);
		}
	} else {
		printf("Can't get audio signal status.\n");
	}
}

void GetVideoColorName(MWCAP_VIDEO_COLOR_FORMAT color, char *pszName, int nSize)
{
	switch (color) {
	case MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN:
		strcpy_s(pszName, nSize, "Unknown");
		break;
	case MWCAP_VIDEO_COLOR_FORMAT_RGB:
		strcpy_s(pszName, nSize, "RGB");
		break;
	case MWCAP_VIDEO_COLOR_FORMAT_YUV601:
		strcpy_s(pszName, nSize, "YUV BT.601");
		break;
	case MWCAP_VIDEO_COLOR_FORMAT_YUV709:
		strcpy_s(pszName, nSize, "YUV BT.709");
		break;
	case MWCAP_VIDEO_COLOR_FORMAT_YUV2020:
		strcpy_s(pszName, nSize, "YUV BT.2020");
		break;
	case MWCAP_VIDEO_COLOR_FORMAT_YUV2020C:
		strcpy_s(pszName, nSize, "YUV BT.2020C");
		break;
	default:
		strcpy_s(pszName, nSize, "Unknown");
		break;
	}
}

//sdi
void GetVideoSDIType(SDI_TYPE type, char *pTypeName, int nSize)
{
	switch (type) {
	case SDI_TYPE_SD:
		strcpy_s(pTypeName, nSize, "SD");
		break;
	case SDI_TYPE_HD:
		strcpy_s(pTypeName, nSize, "HD");
		break;
	case SDI_TYPE_3GA:
		strcpy_s(pTypeName, nSize, "3GA");
		break;
	case SDI_TYPE_3GB_DL:
		strcpy_s(pTypeName, nSize, "3GB_DL");
		break;
	case SDI_TYPE_3GB_DS:
		strcpy_s(pTypeName, nSize, "3GB_DS");
		break;
	case SDI_TYPE_DL_CH1:
		strcpy_s(pTypeName, nSize, "DL_CH1");
		break;
	case SDI_TYPE_DL_CH2:
		strcpy_s(pTypeName, nSize, "DL_CH2");
		break;
	case SDI_TYPE_6G_MODE1:
		strcpy_s(pTypeName, nSize, "6G_MODE1");
		break;
	case SDI_TYPE_6G_MODE2:
		strcpy_s(pTypeName, nSize, "6G_MODE2");
		break;
	case SDI_TYPE_12G:
		strcpy_s(pTypeName, nSize, "12G");
		break;
	case SDI_TYPE_DL_HD:
		strcpy_s(pTypeName, nSize, "DL_HD");
		break;
	case SDI_TYPE_DL_3GA:
		strcpy_s(pTypeName, nSize, "DL_3GA");
		break;
	case SDI_TYPE_DL_3GB:
		strcpy_s(pTypeName, nSize, "DL_3GB");
		break;
	case SDI_TYPE_DL_6G:
		strcpy_s(pTypeName, nSize, "DL_6G");
		break;
	case SDI_TYPE_QL_HD:
		strcpy_s(pTypeName, nSize, "QL_HD");
		break;
	case SDI_TYPE_QL_3GA:
		strcpy_s(pTypeName, nSize, "QL_3GA");
		break;
	case SDI_TYPE_QL_3GB:
		strcpy_s(pTypeName, nSize, "QL_3GB");
		break;
	default:
		strcpy_s(pTypeName, nSize, "Unknown");
		break;
	}
}

void GetVideoScanFmt(SDI_SCANNING_FORMAT type, char *pFmtName, int nSize)
{
	switch (type) {
	case SDI_SCANING_INTERLACED:
		strcpy_s(pFmtName, nSize, "INTERLACED");
		break;
	case SDI_SCANING_SEGMENTED_FRAME:
		strcpy_s(pFmtName, nSize, "SEGMENTED_FRAME");
		break;
	case SDI_SCANING_PROGRESSIVE:
		strcpy_s(pFmtName, nSize, "PROGRESSIVE");
		break;
	default:
		strcpy_s(pFmtName, nSize, "Unknown");
		break;
	}
}

void GetVideoSamplingStruct(SDI_SAMPLING_STRUCT type, char *pStructName,
			    int nSize)
{
	switch (type) {
	case SDI_SAMPLING_422_YCbCr:
		strcpy_s(pStructName, nSize, "422_YCbCr");
		break;
	case SDI_SAMPLING_444_YCbCr:
		strcpy_s(pStructName, nSize, "444_YCbCr");
		break;
	case SDI_SAMPLING_444_RGB:
		strcpy_s(pStructName, nSize, "444_RGB");
		break;
	case SDI_SAMPLING_420_YCbCr:
		strcpy_s(pStructName, nSize, "420_YCbCr");
		break;
	case SDI_SAMPLING_4224_YCbCrA:
		strcpy_s(pStructName, nSize, "4224_YCbCrA");
		break;
	case SDI_SAMPLING_4444_YCbCrA:
		strcpy_s(pStructName, nSize, "4444_YCbCrA");
		break;
	case SDI_SAMPLING_4444_RGBA:
		strcpy_s(pStructName, nSize, "4444_RGBA");
		break;
	case SDI_SAMPLING_4224_YCbCrD:
		strcpy_s(pStructName, nSize, "4224_YCbCrD");
		break;
	case SDI_SAMPLING_4444_YCbCrD:
		strcpy_s(pStructName, nSize, "4444_YCbCrD");
		break;
	case SDI_SAMPLING_4444_RGBD:
		strcpy_s(pStructName, nSize, "4444_RGBD");
		break;
	case SDI_SAMPLING_444_XYZ:
		strcpy_s(pStructName, nSize, "444_XYZ");
		break;
	default:
		strcpy_s(pStructName, nSize, "Unknown");
		break;
	}
}

void GetVideoBitDepth(SDI_BIT_DEPTH type, char *pTypeName, int nSize)
{
	switch (type) {
	case SDI_BIT_DEPTH_8BIT:
		strcpy_s(pTypeName, nSize, "8bit");
		break;
	case SDI_BIT_DEPTH_10BIT:
		strcpy_s(pTypeName, nSize, "10bit");
		break;
	case SDI_BIT_DEPTH_12BIT:
		strcpy_s(pTypeName, nSize, "12bit");
		break;
	default:
		strcpy_s(pTypeName, nSize, "Unknown");
		break;
	}
}

//vga
void GetVideoSyncType(BYTE type, char *pTypeName, int nSize)
{
	switch (type) {
	case VIDEO_SYNC_ALL:
		strcpy_s(pTypeName, nSize, "ALL");
		break;
	case VIDEO_SYNC_HS_VS:
		strcpy_s(pTypeName, nSize, "HS_VS");
		break;
	case VIDEO_SYNC_CS:
		strcpy_s(pTypeName, nSize, "CS");
		break;
	case VIDEO_SYNC_EMBEDDED:
		strcpy_s(pTypeName, nSize, "EMBEDDED");
		break;
	default:
		strcpy_s(pTypeName, nSize, "Unknown");
		break;
	}
}

//cvbs
void GetVideoSDStandard(MWCAP_SD_VIDEO_STANDARD type, char *pTypeName,
			int nSize)
{
	switch (type) {
	case MWCAP_SD_VIDEO_NONE:
		strcpy_s(pTypeName, nSize, "NONE");
		break;
	case MWCAP_SD_VIDEO_NTSC_M:
		strcpy_s(pTypeName, nSize, "NTSC_M");
		break;
	case MWCAP_SD_VIDEO_NTSC_433:
		strcpy_s(pTypeName, nSize, "NTSC_433");
		break;
	case MWCAP_SD_VIDEO_PAL_M:
		strcpy_s(pTypeName, nSize, "PAL_M");
		break;
	case MWCAP_SD_VIDEO_PAL_60:
		strcpy_s(pTypeName, nSize, "PAL_60");
		break;
	case MWCAP_SD_VIDEO_PAL_COMBN:
		strcpy_s(pTypeName, nSize, "PAL_COMBN");
		break;
	case MWCAP_SD_VIDEO_PAL_BGHID:
		strcpy_s(pTypeName, nSize, "PAL_BGHID");
		break;
	case MWCAP_SD_VIDEO_SECAM:
		strcpy_s(pTypeName, nSize, "SECAM");
		break;
	case MWCAP_SD_VIDEO_SECAM_60:
		strcpy_s(pTypeName, nSize, "SECAM_60");
		break;
	default:
		strcpy_s(pTypeName, nSize, "Unknown");
		break;
	}
}

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
	CreateDumpFile(L"InputSignal.dmp", pException);
	return EXCEPTION_EXECUTE_HANDLER;
}