#ifndef CAPTUREBYTIMERECO_H
#define CAPTUREBYTIMERECO_H

#include "stdafx.h"

#include <windows.h>
#include <gdiplus.h>
#include <direct.h>

#include "LibMWCapture\MWCapture.h"
#include "MWCmdCommon.h"

using namespace Gdiplus;

#define NUM_CAPTURE_ECO 100

void CaptureByTimerEco(HCHANNEL t_h_channel, LONGLONG t_ll_duration)
{
	MW_RESULT t_mr = MW_SUCCEEDED;
	MWCAP_CHANNEL_INFO t_channel_info;
	t_mr = MWGetChannelInfo(t_h_channel, &t_channel_info);
	if (t_mr != MW_SUCCEEDED) {
		printf("error:get channel info failed\n");
		return;
	}
	if (t_channel_info.wFamilyID != MW_FAMILY_ID_ECO_CAPTURE) {
		printf("error:unsuitable device type\n");
		return;
	}

	HANDLE t_h_event_capture = CreateEvent(NULL, FALSE, FALSE, NULL);

	// set video eco open
	MWCAP_VIDEO_ECO_CAPTURE_OPEN t_eco_capture_open = {0};
	t_eco_capture_open.cx = 1920;
	t_eco_capture_open.cy = 1080;
	t_eco_capture_open.dwFOURCC = MWFOURCC_BGR24;
	t_eco_capture_open.hEvent = t_h_event_capture;
	t_eco_capture_open.llFrameDuration =
		t_ll_duration; // when set -1,capture by input

	Bitmap bitmap(t_eco_capture_open.cx, t_eco_capture_open.cy,
		      PixelFormat24bppRGB);
	Rect rect(0, 0, t_eco_capture_open.cx, t_eco_capture_open.cy);
	BitmapData bitmapData;

	Status status = bitmap.LockBits(&rect, ImageLockModeWrite,
					PixelFormat24bppRGB, &bitmapData);

	BOOL bBottomUp = FALSE;
	if (bitmapData.Stride < 0) {
		bitmapData.Scan0 =
			((LPBYTE)bitmapData.Scan0) +
			bitmapData.Stride * (t_eco_capture_open.cy - 1);
		bitmapData.Stride = -bitmapData.Stride;
		bBottomUp = TRUE;
	}

	MWCAP_VIDEO_ECO_CAPTURE_FRAME t_video_frame[3] = {0};
	int t_n_stride =
		FOURCC_CalcMinStride(MWFOURCC_BGR24, t_eco_capture_open.cx, 4);
	int t_n_size = FOURCC_CalcImageSize(MWFOURCC_BGR24,
					    t_eco_capture_open.cx,
					    t_eco_capture_open.cy, t_n_stride);
	for (int i = 0; i < sizeof(t_video_frame) / sizeof(t_video_frame[0]);
	     i++) {
		t_video_frame[i].bBottomUp = bBottomUp;
		t_video_frame[i].cbFrame = t_n_size;
		t_video_frame[i].cbStride = t_n_stride;
		t_video_frame[i].deinterlaceMode =
			MWCAP_VIDEO_DEINTERLACE_WEAVE;
		t_video_frame[i].pvFrame = (MWCAP_PTR64)malloc(t_n_size);
		t_video_frame[i].pvContext = &(t_video_frame[(i + 1) % 3]);
	}

	do {
		t_mr = MWStartVideoEcoCapture(t_h_channel, &t_eco_capture_open);
		if (t_mr != MW_SUCCEEDED) {
			printf("error:start video eco capture failed\n");
			break;
		}

		// set video capture frame
		for (int i = 0; i < 3; i++)
			t_mr = MWCaptureSetVideoEcoFrame(t_h_channel,
							 &(t_video_frame[i]));

		LONGLONG t_ll_begin = 0;
		t_mr = MWGetDeviceTime(t_h_channel, &t_ll_begin);
		LONGLONG t_ll_last = t_ll_begin;

		for (int i = 0; i < NUM_CAPTURE_ECO; i++) {
			DWORD t_dw_ret = WaitForSingleObject(t_h_event_capture,
							     INFINITE);
			if (t_dw_ret != WAIT_OBJECT_0)
				break;

			MWCAP_VIDEO_ECO_CAPTURE_STATUS t_status_capture;
			while (i < NUM_CAPTURE_ECO) {
				t_mr = MWGetVideoEcoCaptureStatus(
					t_h_channel, &t_status_capture);
				if (t_mr != MW_SUCCEEDED ||
				    t_status_capture.pvFrame == NULL)
					break;

				memcpy(bitmapData.Scan0,
				       t_status_capture.pvFrame,
				       bitmapData.Stride * bitmapData.Height);

				t_mr = MWCaptureSetVideoEcoFrame(
					t_h_channel,
					(MWCAP_VIDEO_ECO_CAPTURE_FRAME *)
						t_status_capture.pvContext);
			}
		}

		char path[256];
		WCHAR *wPath = NULL;
		BOOL bRet = FALSE;
		bRet = GetPath(path, 256);
		if (bRet != TRUE) {
			printf("\nCan't get the save path.\n");
			return;
		}
		_mkdir(path);

		sprintf_s(path, "%s\\CaptureByTimerEco.bmp", path);
		wPath = AnsiToUnicode(path);

		printf("\nThe last frame is saved in %s.\n\n", path);

		CLSID pngClsid;
		GetEncoderClsid((const wchar_t *)_T("image/bmp"), &pngClsid);
		bitmap.Save(wPath, &pngClsid, NULL);

		if (wPath != NULL) {
			delete[] wPath;
			wPath = NULL;
		}

	} while (false);

	// clean up
	if (t_h_channel) {
		MWStopVideoEcoCapture(t_h_channel);
		t_h_channel = NULL;
	}

	if (t_h_event_capture != NULL) {
		CloseHandle(t_h_event_capture);
		t_h_event_capture = NULL;
	}

	for (int i = 0; i < sizeof(t_video_frame) / sizeof(t_video_frame[0]);
	     i++) {
		if (t_video_frame[i].pvFrame) {
			free(t_video_frame[i].pvFrame);
			t_video_frame[i].pvFrame = NULL;
		}
	}
}

#endif