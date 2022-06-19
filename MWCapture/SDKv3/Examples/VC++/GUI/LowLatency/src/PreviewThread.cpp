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

#include "stdafx.h"
#include "PreviewThread.h"

CPreviewThread::CPreviewThread() 
{
	m_hThread = NULL;
	m_hEvent = NULL;
	m_hChannel = NULL;

	m_pCallback = NULL;
}

CPreviewThread::~CPreviewThread() 
{
}

BOOL CPreviewThread::Create(int nIndex, int cx, int cy, DWORD dwFrameDuration, DWORD dwFOURCC, IPreviewCallback* pCallback)
{
	BOOL bRet = FALSE;
	do {
		MWCAP_CHANNEL_INFO videoInfo = {0};
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nIndex, &videoInfo)) {
			break;
		}

		// Open channel
		TCHAR szDevicePath[MAX_PATH];
		if (MW_SUCCEEDED != MWGetDevicePath(nIndex, szDevicePath)) {
			OutputDebugStringA("ERROR: MWGetDevicePath fail !\n");
			break;
		}
		m_hChannel = MWOpenChannelByPath(szDevicePath);
		if (m_hChannel == NULL) {
			break;
		}
		m_cx = cx;
		m_cy = cy;

		m_dwFrameDuration = dwFrameDuration;
		m_dwFOURCC = dwFOURCC;

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

void CPreviewThread::Destroy()
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

DWORD CPreviewThread::ThreadProc()
{
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	
	DWORD cbStride = FOURCC_CalcMinStride(m_dwFOURCC, m_cx, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(m_dwFOURCC, m_cx, m_cy, cbStride);
	BYTE* byBuffer = new BYTE[dwFrameSize];
	if (m_pCallback != NULL) {
		memset(byBuffer, 0, dwFrameSize);
		m_pCallback->OnVideoCallback(byBuffer, cbStride);
	}

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

		xr = MWGetVideoSignalStatus(m_hChannel, &m_status);
		if (xr != MW_SUCCEEDED)
			break;

		HTIMER hTimer = MWRegisterTimer(m_hChannel, hNotifyEvent);
		if (hTimer == NULL)
			break;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llExpireTime = llBegin;
		int i = 0;
		while (TRUE) {
			llExpireTime = llExpireTime + m_dwFrameDuration;
			xr = MWScheduleTimer(m_hChannel, hTimer, llExpireTime);
			if (xr != MW_SUCCEEDED)
				continue;

			HANDLE aEvent[] = {m_hEvent, hNotifyEvent};
			DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0)
				break;

			xr = MWGetVideoSignalStatus(m_hChannel, &m_status);
			if (xr != MW_SUCCEEDED)
				break;

			xr = MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, &videoFrameInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			xr = MWCaptureVideoFrameToVirtualAddress(m_hChannel, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, byBuffer, dwFrameSize, cbStride, FALSE, NULL, m_dwFOURCC, m_cx, m_cy); 

			WaitForSingleObject(hCaptureEvent, INFINITE);
			xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);

			if (m_pCallback != NULL) {
				m_pCallback->OnVideoCallback(byBuffer, cbStride);
			}

			MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
			xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);
		}

		xr = MWUnregisterTimer(m_hChannel, hTimer);

		xr = MWStopVideoCapture(m_hChannel);

		xr = MWUnpinVideoBuffer(m_hChannel, byBuffer);

	} while (FALSE);

	if (m_pCallback != NULL) {
		memset(byBuffer, 0, dwFrameSize);
		m_pCallback->OnVideoCallback(byBuffer, cbStride);
	}

	delete [] byBuffer;

	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);

	return 0;
}
