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
#include "CaptureThread.h"


CCaptureThread::CCaptureThread() 
{
	m_hTimerThread = NULL;
	m_bExitTimerThread = FALSE;
	m_llTimeGetFirst = 0;
	m_hSignalThread = NULL;
	m_hExitSignalThread = NULL;
	
	m_hVideoThread = NULL;
	m_hExitVideoThread = NULL;

	m_nNumChannel = 0;
	memset(m_arrChannel, 0, sizeof(CHANNEL_ITEM) * MAX_CHANNEL_COUNT);

	m_nValidChannel = 0;
	m_dCurrentFps = 0.0f;

	m_pbBuffer = NULL;
	m_pCallback = NULL;
}

CCaptureThread::~CCaptureThread() 
{
}

BOOL CCaptureThread::Create(const CHANNEL_INDEX* pItems, int nNumChannel, int nLayoutX, int nLayoutY, int cx, int cy, DWORD dwFourcc, DWORD dwFrameDuration, BOOL bSyncMode, ICaptureCallback* pCallback, DWORD dwParam) 
{
	MW_RESULT xr;
	BOOL bRet = FALSE;
	do {
		for (int i = 0; i < nLayoutX * nLayoutY; i ++) {
			if (i >= nNumChannel)
				continue;

			CHANNEL_ITEM& item = m_arrChannel[i];
			item.nIndex = g_nValidChannel[pItems[i].nIndex];
			item.nBoard = pItems[i].nBoard;
			item.nChannel = pItems[i].nChannel;

			TCHAR szDevicePath[MAX_PATH];
			MWGetDevicePath(item.nIndex, szDevicePath);
			item.hChannel = MWOpenChannelByPath(szDevicePath);
			if (item.hChannel == NULL)
				continue;
		
			bRet = FALSE;
			do {
				item.hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (item.hCaptureEvent == NULL)
					break;

				item.hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
				if (item.hNotifyEvent == NULL)
					break;

				xr = MWStartVideoCapture(item.hChannel, item.hCaptureEvent);
				if (xr != MW_SUCCEEDED)
					break;

				DWORD dwNotify = MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED | MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE;
				item.hNotify = MWRegisterNotify(item.hChannel, item.hNotifyEvent, dwNotify);
				if (item.hNotify == NULL)
					break;

				m_nValidChannel ++;

				bRet = TRUE;
			} while (FALSE);

			if (!bRet) {
				MWStopVideoCapture(item.hChannel);
				MWCloseChannel(item.hChannel);
				item.hChannel = NULL;

				if (item.hCaptureEvent != NULL) {
					CloseHandle(item.hCaptureEvent);
					item.hCaptureEvent = NULL;
				}
				if (item.hNotifyEvent != NULL) {
					CloseHandle(item.hNotifyEvent);
					item.hNotifyEvent = NULL;
				}
			}

			// set position
			int x = i % nLayoutX;
			int y = i / nLayoutX;

			int w = cx;
			int h = cy;

			item.rcPos.left		= x * w;
			item.rcPos.right	= (x + 1) * w;
			item.rcPos.top		= y * h;
			item.rcPos.bottom	= (y + 1) * h;
		}

		DWORD dwStride = FOURCC_CalcMinStride(dwFourcc, cx * nLayoutX, 4);
		m_cbBuffer = FOURCC_CalcImageSize(dwFourcc, cx * nLayoutX, cy * nLayoutY, dwStride);
		m_pbBuffer = new BYTE[m_cbBuffer];
		memset(m_pbBuffer, 0, m_cbBuffer);

		m_pCallback = pCallback;
		m_dwParam	= dwParam;

		m_nLayoutX = nLayoutX;
		m_nLayoutY = nLayoutY;

		m_cx = cx;
		m_cy = cy;

		m_dwFourcc	= dwFourcc;
		m_dwFrameDuration = dwFrameDuration;

		m_bSyncMode = bSyncMode;

		m_nNumChannel = nNumChannel;

		if (m_bSyncMode) {
			m_hExitSignalThread = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (m_hExitSignalThread == NULL)
				break;

			m_hSignalThread = CreateThread(NULL, 0, SignalThreadProc, (LPVOID)this, 0, NULL);
			if (m_hSignalThread == NULL)
				break;

			m_bExitTimerThread = FALSE;
			m_hTimerThread = CreateThread(NULL, 0, TimerThreadProc, (LPVOID)this, 0, NULL);
			if (m_hTimerThread == NULL)
				break;
		}

		m_hExitVideoThread = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hExitVideoThread == NULL)
			break;

		m_hVideoThread = CreateThread(NULL, 0, VideoThreadProc, (LPVOID)this, 0, NULL);
		if (m_hVideoThread == NULL)
			break;

		bRet = TRUE;
	} while (FALSE);

	if (!bRet) {
		Destroy();
	}
	return TRUE;
}

void CCaptureThread::Destroy() 
{
	if (m_hSignalThread != NULL) {
		if (m_hExitSignalThread != NULL) {
			SetEvent(m_hExitSignalThread);
			WaitForSingleObject(m_hSignalThread, INFINITE);
			CloseHandle(m_hExitSignalThread);
			m_hExitSignalThread = NULL;
		}
		CloseHandle(m_hSignalThread);
		m_hSignalThread = NULL;
	}
	if (m_hExitVideoThread != NULL) {
		if (m_hExitVideoThread != NULL) {
			SetEvent(m_hExitVideoThread);
			WaitForSingleObject(m_hVideoThread, INFINITE);
			CloseHandle(m_hExitVideoThread);
			m_hExitVideoThread = NULL;
		}
		CloseHandle(m_hVideoThread);
		m_hVideoThread = NULL;
	}

	if (m_hTimerThread != NULL) {
		if (m_bExitTimerThread != TRUE) {
			m_bExitTimerThread = TRUE;
			WaitForSingleObject(m_hTimerThread, INFINITE);
			m_bExitTimerThread = FALSE;
		}
		CloseHandle(m_hTimerThread);
		m_hTimerThread = NULL;
	}

	for (int i = 0; i < m_nNumChannel; i ++) {
		CHANNEL_ITEM& item = m_arrChannel[i];
		if (item.hNotify != NULL) {
			MWUnregisterNotify(item.hChannel, item.hNotify);
			item.hNotify = NULL;
		}
		if (item.hChannel != NULL) {
			MWStopVideoCapture(item.hChannel);
			MWCloseChannel(item.hChannel);
			item.hChannel = NULL;
		}
		if (item.hCaptureEvent != NULL) {
			CloseHandle(item.hCaptureEvent);
			item.hCaptureEvent = NULL;
		}
		if (item.hNotifyEvent != NULL) {
			CloseHandle(item.hNotifyEvent);
			item.hNotifyEvent = NULL;
		}
	}

	if (m_pbBuffer != NULL) {
		delete [] m_pbBuffer;
		m_pbBuffer = NULL;
	}

	m_nValidChannel = 0;
}

DWORD CCaptureThread::TimerThreadProc()
{
	while(!m_bExitTimerThread)
	{
		LONGLONG llTemp = GetTickCount();
		if ((llTemp - m_llTimeGetFirst) >= 5)
		{
			CMWAutoLock lock(m_lockGetFirst);
			m_bGetFirst = FALSE;
		}
	}

	return 0;
}

DWORD CCaptureThread::SignalThreadProc()
{
	SetPriorityClass(m_hSignalThread, BELOW_NORMAL_PRIORITY_CLASS);

	HANDLE	arrEvent[MAX_CHANNEL_COUNT + 1];
	int		arrIndex[MAX_CHANNEL_COUNT];
	int		nCount = 0;

	MW_RESULT xr;

	m_dwStdMask = 0;
	m_dwCurMask = 0;
	m_dwCapMask = 0;

	LONGLONG llStandardTime = 0LL;
	for (int i = 0; i < m_nNumChannel; i ++) {
		CHANNEL_ITEM& item = m_arrChannel[i];
		if (item.hChannel == NULL || item.hCaptureEvent == NULL || item.hNotifyEvent == NULL || item.hNotify == NULL)
			continue;

		// sync channel clock
		if (llStandardTime == 0LL)
			xr = MWGetDeviceTime(item.hChannel, &llStandardTime);
		else
			xr = MWSetDeviceTime(item.hChannel, llStandardTime);

		if (xr != MW_SUCCEEDED)
			continue;

		arrEvent[nCount] = item.hNotifyEvent;
		arrIndex[nCount] = i;
		nCount ++;

		MWCAP_VIDEO_SIGNAL_STATUS signalStatus;
		xr = MWGetVideoSignalStatus(item.hChannel, &signalStatus);
		if (xr != MW_SUCCEEDED)
			continue;

		if (signalStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
			m_dwStdMask |= (1 << i);
		}
	}
	arrEvent[nCount] = m_hExitSignalThread;

	if (m_dwStdMask == 0)
		return 0;

	LONGLONG	llStdTime = 0LL;
	DWORD		dwStdFrameDuration = 0;

	while (TRUE) {
		//获取一个通道
		DWORD dwRet = WaitForMultipleObjects(nCount + 1, arrEvent, FALSE, INFINITE);
		int nPos = (dwRet - WAIT_OBJECT_0);
		if (nPos == nCount)
			break;

		int nChannelIndex = arrIndex[nPos];
		CHANNEL_ITEM& item = m_arrChannel[nChannelIndex];
		ULONGLONG ullStatusBits = 0;
		xr = MWGetNotifyStatus(item.hChannel, item.hNotify, &ullStatusBits);
		if (xr != MW_SUCCEEDED)
			continue;

		MWCAP_VIDEO_SIGNAL_STATUS signalStatus;
		xr = MWGetVideoSignalStatus(item.hChannel, &signalStatus);
		if (xr != MW_SUCCEEDED)
			continue;

		//如果这个通道信号变化了，则更新m_stdMask
		if (ullStatusBits & MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE) {
			if (signalStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
				m_dwStdMask |= (1 << nChannelIndex);
			}
			else {
				m_dwStdMask &= ~(1 << nChannelIndex);
			}
		}

		if (ullStatusBits & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) {
			MWCAP_VIDEO_BUFFER_INFO buffInfo;
			xr = MWGetVideoBufferInfo(item.hChannel, &buffInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			//获取得到的这个通道的帧信息
			MWCAP_VIDEO_FRAME_INFO frameInfo;
			xr = MWGetVideoFrameInfo(item.hChannel, buffInfo.iNewestBuffered, &frameInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			if (m_bGetFirst == FALSE)
			{
				m_llTimeGetFirst = GetTickCount();
				llStdTime = frameInfo.allFieldStartTimes[0];
				dwStdFrameDuration = signalStatus.dwFrameDuration;

				CMWAutoLock lock(m_lockGetFirst);
				m_bGetFirst = TRUE;
				m_dwCurMask = (1 << nChannelIndex);
			}
			else if (m_dwCurMask != m_dwStdMask)
			{
				if (MWCheckTime(llStdTime, frameInfo.allFieldStartTimes[0], dwStdFrameDuration, 0.1f)) {
					m_dwCurMask |= (1 << nChannelIndex);
				}
			}

			item.byCurIndex = buffInfo.iNewestBuffered;

			if (m_dwCurMask == m_dwStdMask) {
				CMWAutoLock lock(m_lock);

				for (int i = 0; i < m_nNumChannel; i ++) {
					CHANNEL_ITEM& item = m_arrChannel[i];
					if (item.hChannel == NULL || item.hCaptureEvent == NULL)
						continue;

					DWORD dwMask = (1 << i);
					if (!(m_dwCurMask & dwMask))
						continue;

					item.byCapIndex = item.byCurIndex;
				}
				m_dwCapMask = m_dwCurMask;
			}
		}
	}

	return 0;
}

DWORD CCaptureThread::VideoThreadProc() 
{
	int nWidth = m_cx * m_nLayoutX;
	int nHeight = m_cy * m_nLayoutY;

	int nStride = FOURCC_CalcMinStride(m_dwFourcc, nWidth, 4);

	int nFrameCount = 0;
	int nFPSCount = 0;

	HCHANNEL hChannel = NULL;
	for (int i = 0; i < m_nNumChannel; i ++) {
		CHANNEL_ITEM& item = m_arrChannel[i];
		if (item.hChannel != NULL) {
			hChannel = item.hChannel;
			break;
		}
	}
	if (hChannel == NULL)
		return 0;

	HANDLE hTimerEvent	= CreateEvent(NULL, FALSE, FALSE, NULL);
	HTIMER hTimerNotify = MWRegisterTimer(hChannel, hTimerEvent);

	MW_RESULT xr;
	LONGLONG llBegin = 0LL;
	xr = MWGetDeviceTime(hChannel, &llBegin);

	LONGLONG llExpireTime = llBegin;
	LONGLONG llLast = llBegin;
	while (TRUE) {
		llExpireTime += m_dwFrameDuration;

		xr = MWScheduleTimer(hChannel, hTimerNotify, llExpireTime);
		if (xr != MW_SUCCEEDED) {
			continue;
		}

		HANDLE aEventNotify[2] = {m_hExitVideoThread, hTimerEvent};
		DWORD dwRet = WaitForMultipleObjects(2, aEventNotify, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED) {
			break;
		}
		else if (dwRet == WAIT_OBJECT_0 + 1) {
			int nNumEvent = 0;
			HANDLE arrEvent[MAX_CHANNEL_COUNT];

			// capture video
			if (m_bSyncMode) {
				CMWAutoLock lock(m_lock);

				for (int i = 0; i < m_nNumChannel; i ++) {
					DWORD dwMask = (1 << i);
					if (!(m_dwCapMask & dwMask))
						continue;

					CHANNEL_ITEM& item = m_arrChannel[i];
					xr = MWCaptureVideoFrameToVirtualAddressEx(item.hChannel, item.byCapIndex, m_pbBuffer, m_cbBuffer, nStride, FALSE, NULL, m_dwFourcc,
						nWidth, nHeight, 0, 0, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_BLEND, MWCAP_VIDEO_ASPECT_RATIO_IGNORE, 
						NULL, &(item.rcPos), 0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN); 
					if (xr != MW_SUCCEEDED)
						continue;

					arrEvent[nNumEvent++] = item.hCaptureEvent;
				}

				m_dwCapMask = 0;
			}
			else {
				for (int i = 0; i < m_nNumChannel; i ++) {
					CHANNEL_ITEM& item = m_arrChannel[i];
					if (item.hChannel == NULL)
						continue;

					xr = MWCaptureVideoFrameToVirtualAddressEx(item.hChannel, MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, m_pbBuffer, m_cbBuffer, nStride, FALSE, NULL, m_dwFourcc,
						nWidth, nHeight, 0, 0, NULL, NULL, 0, 100, 0, 100, 0, MWCAP_VIDEO_DEINTERLACE_BLEND, MWCAP_VIDEO_ASPECT_RATIO_IGNORE, 
						NULL, &(item.rcPos), 0, 0, MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN, MWCAP_VIDEO_QUANTIZATION_UNKNOWN, MWCAP_VIDEO_SATURATION_UNKNOWN); 
					if (xr != MW_SUCCEEDED)
						continue;

					arrEvent[nNumEvent++] = item.hCaptureEvent;
				}
			}

			WaitForMultipleObjects(nNumEvent, arrEvent, TRUE, INFINITE);

			for (int i = 0; i < m_nNumChannel; i ++) {
				CHANNEL_ITEM& item = m_arrChannel[i];
				if (item.hChannel != NULL) {
					MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
					MWGetVideoCaptureStatus(item.hChannel, &captureStatus);
				}
			}

			if (m_pCallback != NULL) {
				m_pCallback->OnCaptureCallback(m_pbBuffer, nStride, m_dwParam);
			}

			nFrameCount ++;
			
			if (m_bSyncMode && (nFrameCount % 100 == 0)) {
				LONGLONG llStandardTime = 0LL;
				for (int i = 0; i < m_nNumChannel; i ++) {
					CHANNEL_ITEM& item = m_arrChannel[i];
					if (item.hChannel == NULL)
						continue;

					// sync channel clock
					if (llStandardTime == 0LL)
						xr = MWGetDeviceTime(item.hChannel, &llStandardTime);
					else
						xr = MWRegulateDeviceTime(item.hChannel, llStandardTime);
				}
			}

			LONGLONG llCurrent = 0LL;
			MWGetDeviceTime(hChannel, &llCurrent);

			nFPSCount ++;

			if (nFPSCount % 10 == 0) {
				m_dCurrentFps = (double)nFPSCount * 10000000LL / (llCurrent - llLast);

				if (llCurrent - llLast > 30000000LL) {
					llLast = llCurrent;
					nFPSCount = 0;
				}
			}
		}
	}

	if (hTimerNotify != NULL) {
		xr = MWUnregisterTimer(hChannel, hTimerNotify);
	}

	if (hTimerEvent != NULL) {
		CloseHandle(hTimerEvent);
	}

	return 0;
}
