#ifndef CAPTUREBYINPUTPRO_H
#define CAPTUREBYINPUTPRO_H

#include "stdafx.h"

#include <windows.h>
#include <gdiplus.h>
#include <direct.h>

#include "LibMWCapture\MWCapture.h"
#include "MWCmdCommon.h"

using namespace Gdiplus;

#define NUM_CAPTURE_PRO 100

//CapturePFrame:
//1.create notify event and capture event to receive capture messages;
//2.call the start function ¡ª¡ª MWStartVideoCapture(hChannel, hCaptureEvent);
//3.register notify for notify event to receive notifies from capture card;
//4.wait for completed notify(MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) and use the function MWCaptureVideoFrameToVirtualAddressEx to save data
//  ,WaitForSingleObject(hCaptureEvent, INFINITE) and hCaptureEvent means all data received ,then you can do what you want;
//5.call function MWGetVideoCaptureStatus to free memory source
//6.in our example, we save the video data as .bmp, this is not necessary, so we have to calling function(GetEncoderClsid) to get the CLSID and calling Bitmap::Save to save bitmap;
//7.close capture and unregister notify.

// Tips : in our example, we add the var(llTotalTime and llCurrent) to calculate the time in capturing, but this is not necessary.
void CapturePFramePro(HCHANNEL t_h_channel)
{
	MW_RESULT t_mr = MW_SUCCEEDED;
	MWCAP_CHANNEL_INFO t_channel_info;
	t_mr = MWGetChannelInfo(t_h_channel, &t_channel_info);
	if (t_mr != MW_SUCCEEDED) {
		printf("error:get channel info failed\n");
		return;
	}
	if (t_channel_info.wFamilyID != MW_FAMILY_ID_PRO_CAPTURE) {
		printf("error:unsuitable device type\n");
		return;
	}

	MWCAP_VIDEO_SIGNAL_STATUS t_video_signal_status;
	MWGetVideoSignalStatus(t_h_channel, &t_video_signal_status);

	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	t_mr = MWStartVideoCapture(t_h_channel, hCaptureEvent);
	if (t_mr != MW_SUCCEEDED) {
		printf("ERROR: Open Video Capture error!\n");
	} else {
		MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
		MWGetVideoBufferInfo(t_h_channel, &videoBufferInfo);

		MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
		MWGetVideoFrameInfo(t_h_channel,
				    videoBufferInfo.iNewestBufferedFullFrame,
				    &videoFrameInfo);

		if (t_video_signal_status.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
			// Allocate capture buffer
			int cx = t_video_signal_status.cx;
			int cy = t_video_signal_status.cy;

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
				t_h_channel, hNotifyEvent,
				MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED);
			if (hNotify == NULL) {
				printf("ERROR: Register Notify error.\n");
			} else {
				double fps =
					(double)10000000LL /
					t_video_signal_status.dwFrameDuration;
				printf("Begin to capture %d frames by %.2f fps...\n",
				       NUM_CAPTURE_PRO, fps);

				LONGLONG llTotalTime = 0LL;

				MWPinVideoBuffer(t_h_channel,
						 (LPBYTE)bitmapData.Scan0,
						 bitmapData.Stride * cy);
				for (int i = 0; i < NUM_CAPTURE_PRO; i++) {
					WaitForSingleObject(hNotifyEvent,
							    INFINITE);

					ULONGLONG ullStatusBits = 0;
					t_mr = MWGetNotifyStatus(
						t_h_channel, hNotify,
						&ullStatusBits);
					if (t_mr != MW_SUCCEEDED)
						continue;

					t_mr = MWGetVideoBufferInfo(
						t_h_channel, &videoBufferInfo);
					if (t_mr != MW_SUCCEEDED)
						continue;

					t_mr = MWGetVideoFrameInfo(
						t_h_channel,
						videoBufferInfo
							.iNewestBufferedFullFrame,
						&videoFrameInfo);
					if (t_mr != MW_SUCCEEDED)
						continue;

					if (ullStatusBits &
					    MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) {
						MWCAP_VIDEO_DEINTERLACE_MODE mode =
							MWCAP_VIDEO_DEINTERLACE_WEAVE;

						t_mr = MWCaptureVideoFrameToVirtualAddressEx(
							t_h_channel,
							videoBufferInfo
								.iNewestBufferedFullFrame,
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
						t_mr = MWGetDeviceTime(
							t_h_channel,
							&llCurrent);

						llTotalTime +=
							(llCurrent -
							 (t_video_signal_status
									  .bInterlaced
								  ? videoFrameInfo
									    .allFieldBufferedTimes
										    [1]
								  : videoFrameInfo
									    .allFieldBufferedTimes
										    [0]));

						MWCAP_VIDEO_CAPTURE_STATUS
							t_status;
						MWGetVideoCaptureStatus(
							t_h_channel, &t_status);
					}
				}
				MWUnpinVideoBuffer(t_h_channel,
						   (LPBYTE)bitmapData.Scan0);
				printf("End capture.\n");

				printf("Each frame average capture duration is %d ms.\n",
				       (LONG)(llTotalTime /
					      (NUM_CAPTURE_PRO * 10000)));

				t_mr = MWUnregisterNotify(t_h_channel, hNotify);

				t_mr = MWStopVideoCapture(t_h_channel);

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

				sprintf_s(path, "%s\\CaptureByInputPro.bmp",
					  path);
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

	MWStopVideoCapture(t_h_channel);
}

#endif