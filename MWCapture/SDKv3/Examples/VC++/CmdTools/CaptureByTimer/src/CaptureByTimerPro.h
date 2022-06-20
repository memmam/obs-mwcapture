#ifndef CAPTUREBYTIMERPRO_H
#define CAPTUREBYTIMERPRO_H

#include "stdafx.h"

#include <windows.h>
#include <gdiplus.h>
#include <direct.h>

#include "LibMWCapture\MWCapture.h"
#include "MWCmdCommon.h"

using namespace Gdiplus;

#define NUM_CAPTURE_PRO 100

//start capture:
//1.create notify event and capture event to accept capture message;
//2.set the scan mode as deinterlace;
//3.calling the start function ¡ª¡ª MWStartVideoCapture(hChannel, hCaptureEvent);
//4.register time event and set schedule time to acquire data;
//5.read data by calling MWCaptureVideoFrameToVirtualAddressEx function when the specified time is reached, then you can get the video data and do what you want.
//  in our example, we save the video data as .bmp file. finally, we stop the capture and close the handle.

//Tips : 1)in our example, we add the var(llBegin and llEnd) to record the elapse time in capture, but this is no necessary.

void CaptureByTimerPro(HCHANNEL t_h_channel,LONGLONG t_ll_duration)
{
	MW_RESULT t_mr = MW_SUCCEEDED;
	MWCAP_CHANNEL_INFO t_channel_info;
	t_mr = MWGetChannelInfo(t_h_channel,&t_channel_info);
	if(t_mr!=MW_SUCCEEDED){
		printf("error:get channel info failed\n");
		return;
	}
	if(t_channel_info.wFamilyID!=MW_FAMILY_ID_PRO_CAPTURE){
		printf("error:unsuitable device type\n");
		return;
	}

	// Capture frames on input signal frequency
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	t_mr = MWStartVideoCapture(t_h_channel, hCaptureEvent);
	if (t_mr != MW_SUCCEEDED) 
	{
		printf("ERROR: Open Video Capture error!\n");
	}
	else
	{
		// Allocate capture buffer
		int cx = 1920;
		int cy = 1080;

		Bitmap bitmap(cx, cy, PixelFormat24bppRGB);
		Rect rect(0, 0, cx, cy);
		BitmapData bitmapData;

		Status status = bitmap.LockBits(
			&rect,
			ImageLockModeWrite,
			PixelFormat24bppRGB,
			&bitmapData
			);

		BOOL bBottomUp = FALSE;
		if (bitmapData.Stride < 0) {
			bitmapData.Scan0 = ((LPBYTE)bitmapData.Scan0) + bitmapData.Stride * (cy - 1);
			bitmapData.Stride = -bitmapData.Stride;
			bBottomUp = TRUE;
		}

		HTIMER hTimerEvent = MWRegisterTimer(t_h_channel, hNotifyEvent);

		printf("Begin capture %d frames in 1920 x 1080, RGB24, %d fps...\n", NUM_CAPTURE_PRO, 10000000 / t_ll_duration);

		LONGLONG llBegin = 0LL;
		t_mr = MWGetDeviceTime(t_h_channel, &llBegin);

		MWPinVideoBuffer(t_h_channel,(LPBYTE)bitmapData.Scan0,bitmapData.Stride * cy);
		for (int i = 0; i < NUM_CAPTURE_PRO; i++) 
		{
			t_mr = MWScheduleTimer(t_h_channel, hTimerEvent, llBegin + i * t_ll_duration);
			if (t_mr != MW_SUCCEEDED)
				continue;

			WaitForSingleObject(hNotifyEvent, INFINITE);

			MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
			t_mr = MWGetVideoBufferInfo(t_h_channel, &videoBufferInfo);
			if (t_mr != MW_SUCCEEDED)
				continue;

			MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
			t_mr = MWGetVideoFrameInfo(t_h_channel, videoBufferInfo.iNewestBufferedFullFrame, &videoFrameInfo);
			if (t_mr != MW_SUCCEEDED)
				continue;

			MWCAP_VIDEO_DEINTERLACE_MODE mode=MWCAP_VIDEO_DEINTERLACE_WEAVE;

			t_mr = MWCaptureVideoFrameToVirtualAddressEx(
				t_h_channel, 
				MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, 
				(LPBYTE)bitmapData.Scan0, 
				bitmapData.Stride * cy,
				bitmapData.Stride, 
				bBottomUp, 
				NULL,
				MWFOURCC_BGR24, 
				cx,
				cy,
				0, 
				0,									//partical notify
				NULL,								//OSD
				NULL,								//OSD rect
				0, 
				100, 
				0, 
				100, 
				0, 
				mode,								//deinterlace mode
				MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
				NULL,
				NULL,
				0,
				0,
				MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
				MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
				MWCAP_VIDEO_SATURATION_UNKNOWN
				);
			WaitForSingleObject(hCaptureEvent, INFINITE);

			MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
			t_mr = MWGetVideoCaptureStatus(t_h_channel, &captureStatus);
		}
		LONGLONG llEnd = 0LL;
		t_mr = MWGetDeviceTime(t_h_channel, &llEnd);

		MWUnpinVideoBuffer(t_h_channel,(LPBYTE)bitmapData.Scan0);

		printf("End capture, the duration is %d ms. \n", (llEnd - llBegin) / 10000);

		printf("Each frame average duration is %d (100 ns).\n", (LONG)((llEnd - llBegin) / NUM_CAPTURE_PRO));

		t_mr = MWUnregisterTimer(t_h_channel, hTimerEvent);

		t_mr = MWStopVideoCapture(t_h_channel);

		bitmap.UnlockBits(&bitmapData);

		char path[256];
		WCHAR *wPath = NULL;
		BOOL bRet = FALSE;
		bRet = GetPath(path,256);
		if(bRet!=TRUE){
			printf("\nCan't get the save path.\n");
			return;
		}
		_mkdir(path);
		sprintf_s(path, "%s\\CaptureByTimerPro.bmp", path);
		wPath = AnsiToUnicode(path);

		printf("\nThe last frame is saved in %s.\n\n", path);

		CLSID pngClsid;
		GetEncoderClsid((const wchar_t*)_T("image/bmp"), &pngClsid);
		bitmap.Save(wPath, &pngClsid, NULL);

		if(wPath != NULL)
		{
			delete[] wPath;
			wPath = NULL;
		}
	}	

	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);

	//GdiplusShutdown(gdiplusToken);
}

#endif