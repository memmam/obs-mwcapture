/************************************************************************************************/
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

#include "capturethread.h"

CCaptureThread::CCaptureThread()
{
	m_pCallback = NULL;
	m_hChannel = NULL;
	m_hThread = NULL;
	m_hEvent = NULL;

	//
	m_cx = CAPTURE_WIDTH;
	m_cy = CAPTURE_HEIGHT;
	m_dwFOURCC = CAPTURE_COLOR;
	m_dwFrameDuration = CAPTURE_DURATION;

	m_dwMissingFrameNumber = 0;
	m_nLastFrameIndex = -1;
	m_signalStatus = 0;
}

CCaptureThread::~CCaptureThread()
{

}

int CCaptureThread::GetChannelCount()
{
	return MWGetChannelCount();
}

BOOL CCaptureThread::Create(int boardIndex, int channelIndex, ICaptureCallback* pCallback)
{
	BOOL bRet = FALSE;
	do {
		m_hChannel = MWOpenChannel(boardIndex,channelIndex);
		if (m_hChannel == NULL) {
			printf("Open Channel error\n");
			break;
		}

		MWCAP_VIDEO_SIGNAL_STATUS vStatus;
		if (MW_SUCCEEDED != MWGetVideoSignalStatus(m_hChannel, &vStatus)){
			printf("Get Video Signal Status error\n");
			break;
		}
		m_signalStatus = vStatus.state;
		m_dwFrameDuration = vStatus.dwFrameDuration;
		m_cx = vStatus.cx;
		m_cy = vStatus.cy;
		m_dwFOURCC = CAPTURE_COLOR;
		m_pCallback = pCallback;

		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0, NULL);
		if (m_hThread == NULL) {
			break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (!bRet) {
		Destroy();
		return FALSE;
	}

	return bRet;
}

void CCaptureThread::Destroy()
{
	if (m_hEvent != NULL && m_hThread != NULL) {
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}
}

DWORD CCaptureThread::GetCaptureEndStatus()
{
	printf("End: capture by frame , %d frame(s) missing .\n",m_dwMissingFrameNumber);

	return m_dwMissingFrameNumber; 
}

DWORD CCaptureThread::ThreadProcEx()
{
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD cbStride = FOURCC_CalcMinStride(m_dwFOURCC, m_cx, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(m_dwFOURCC, m_cx, m_cy, cbStride);
	BYTE* byBuffer = new BYTE[dwFrameSize];

	MW_RESULT xr;
	do {
		xr = MWPinVideoBuffer(m_hChannel, byBuffer, dwFrameSize);
		if (xr != MW_SUCCEEDED)
			break;

		xr = MWStartVideoCapture(m_hChannel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;
		
		MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
		if (MW_SUCCEEDED != MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo))
			break;

		MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
		xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, &videoFrameInfo);
		if (xr != MW_SUCCEEDED)
			break;
		
		// LowLatency:	MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING
		// Normal:		 MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED
		HNOTIFY hNotify = MWRegisterNotify(m_hChannel, hNotifyEvent,
			MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED | MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE | MWCAP_NOTIFY_VIDEO_INPUT_SOURCE_CHANGE);
		if (hNotify == NULL)
			break;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		xr = MWGetVideoSignalStatus(m_hChannel, &m_status);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llExpireTime = llBegin;
		int i = 0;
		while (TRUE) {
			BYTE *encodeBuffer = NULL;
			HANDLE aEvent[] = { m_hEvent, hNotifyEvent };
			DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0)
				break;

			xr = MWGetVideoSignalStatus(m_hChannel, &m_status);
			if (xr != MW_SUCCEEDED)
				break;

			ULONGLONG ullStatusBits = 0;
			xr = MWGetNotifyStatus(m_hChannel, hNotify, &ullStatusBits);
			if (xr != MW_SUCCEEDED)
				continue;

			xr = MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			if (ullStatusBits & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) {
				xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, &videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				//get encode buffer for use
				if(m_pCallback != NULL)
					encodeBuffer = m_pCallback->Lock();
				if(encodeBuffer == NULL)
				{
					xr = MWCaptureVideoFrameToVirtualAddress(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, byBuffer, dwFrameSize, cbStride, FALSE, NULL, m_dwFOURCC, m_cx, m_cy);
				}
				else
				{
					//Monitor frames 
					if(m_nLastFrameIndex == -1)
					{
						m_nLastFrameIndex = videoBufferInfo.iNewestBufferedFullFrame;
					}
					else
					{
						m_dwMissingFrameNumber += ((videoBufferInfo.iNewestBufferedFullFrame - m_nLastFrameIndex + videoBufferInfo.cMaxFrames)%videoBufferInfo.cMaxFrames - 1);
						m_nLastFrameIndex = videoBufferInfo.iNewestBufferedFullFrame;
					}

					xr = MWCaptureVideoFrameToVirtualAddress(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, encodeBuffer, dwFrameSize, cbStride, FALSE, NULL, m_dwFOURCC, m_cx, m_cy);
				}
				WaitForSingleObject(hCaptureEvent, INFINITE);
			}
			else if (ullStatusBits & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING) {
				xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBuffering, &videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				//get encode buffer for use
				if(m_pCallback != NULL)
					encodeBuffer = m_pCallback->Lock();
				if(encodeBuffer == NULL)
				{
					xr = MWCaptureVideoFrameToVirtualAddressEx(m_hChannel, videoBufferInfo.iNewestBuffering, byBuffer, dwFrameSize, cbStride, FALSE, NULL, m_dwFOURCC, m_cx, m_cy,
						0, CAPTURE_PART_LINE, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_BLEND, MWCAP_VIDEO_ASPECT_RATIO_IGNORE, NULL, NULL, 0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN);
				}
				else
				{
					//Monitor frames 
					if(m_nLastFrameIndex == -1)
					{
						m_nLastFrameIndex = videoBufferInfo.iNewestBufferedFullFrame;
					}
					else
					{
						m_dwMissingFrameNumber += ((videoBufferInfo.iNewestBufferedFullFrame - m_nLastFrameIndex + videoBufferInfo.cMaxFrames)%videoBufferInfo.cMaxFrames - 1);
						m_nLastFrameIndex = videoBufferInfo.iNewestBufferedFullFrame;
					}

					xr = MWCaptureVideoFrameToVirtualAddressEx(m_hChannel, videoBufferInfo.iNewestBuffering, encodeBuffer, dwFrameSize, cbStride, FALSE, NULL, m_dwFOURCC, m_cx, m_cy,
						0, CAPTURE_PART_LINE, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_BLEND, MWCAP_VIDEO_ASPECT_RATIO_IGNORE, NULL, NULL, 0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN);
				}

				BOOL bFirstSlice = TRUE;
				MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
				do {
					WaitForSingleObject(hCaptureEvent, INFINITE);
					xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);
				} while (xr == MW_SUCCEEDED && !captureStatus.bFrameCompleted);

				xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBuffering, &videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;
			}
			if(m_pCallback != NULL && encodeBuffer != NULL)
			{
				
				m_pCallback->UnLock();
			}

			xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);
		}

		xr = MWUnregisterNotify(m_hChannel, hNotify);
		xr = MWStopVideoCapture(m_hChannel);
		xr = MWUnpinVideoBuffer(m_hChannel, byBuffer);

	} while (FALSE);

	delete[] byBuffer;
	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);

	return 0;
}
