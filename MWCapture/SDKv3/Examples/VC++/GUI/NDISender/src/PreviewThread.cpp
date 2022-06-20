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
#include "StdAfx.h"
#include "PreviewThread.h"


CPreviewThread::CPreviewThread(void)
{
	//video
	m_cx				= 0;
	m_cy				= 0;
	m_dwFOURCC			= 0;
	m_dwFrameDuration	= 0;
	m_hThread		= NULL;
	m_hEvent		= NULL;
	m_hChannel		= NULL;
	m_pCallback		= NULL;
	memset(&m_status, 0, sizeof(MWCAP_VIDEO_SIGNAL_STATUS));

	//audio
	m_nPresentChannel		= 0;
	m_nBitDepthInByte		= 0;
	m_cBitsPerSample		= 0;
	m_dwSampleRate			= 44100;
	m_bIsLPCM				= FALSE;
	m_bAudioValid			= FALSE;
	m_bAudioCapturing		= FALSE;
	m_bPreviewFlag			= FALSE;
	m_hAudioEvent			= NULL;
	m_hAudioCaptureThread	= NULL;
}


CPreviewThread::~CPreviewThread(void)
{
}

BOOL CPreviewThread::Create(int nIndex, int cx, int cy, DWORD dwFrameDuration, DWORD dwFOURCC, BOOL bPreviewFlag, IPreviewCallback* pCallback)
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

		m_bPreviewFlag = bPreviewFlag;
		m_pCallback = pCallback;

		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_hThread = CreateThread(NULL, 0, VideoCaptureThreadProc, (LPVOID)this, 0, NULL);
		if (m_hThread == NULL) {
			break;
		}

		m_hAudioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (MW_SUCCEEDED != MWStartAudioCapture(m_hChannel)) {
			return FALSE;
		}

		//audio
		DWORD dwInputCount = 0;
		if (MW_SUCCEEDED != MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount))
			break;

		if (MW_SUCCEEDED != MWGetAudioSignalStatus(m_hChannel, &m_audioSignalStatus)) {
			return FALSE;
		}

		m_bAudioValid		= (dwInputCount > 0 && ((m_audioSignalStatus.wChannelValid & 1) != 0));
		m_bIsLPCM			= m_audioSignalStatus.bLPCM;
		m_cBitsPerSample	= m_audioSignalStatus.cBitsPerSample;
		m_dwSampleRate		= m_audioSignalStatus.dwSampleRate;

		m_nBitDepthInByte = m_audioSignalStatus.cBitsPerSample / 8;
		//m_nBitDepthInByte = 4;
		if (m_audioSignalStatus.bLPCM) {
			m_bIsLPCM = TRUE;
		}
		else {
			m_bIsLPCM = FALSE;
		}

		m_nPresentChannel = 0;
		for (int i = 0; i < 4; ++i) {
			BOOL bPresent = (m_audioSignalStatus.wChannelValid & (0x01 << i)) ? TRUE : FALSE;
			m_nPresentChannel += bPresent ? 2 : 0;
		}
		
		m_bAudioCapturing = TRUE;
		m_hAudioCaptureThread = CreateThread(NULL, 0, AudioCaptureThreadProc, (LPVOID)this, 0, NULL);
		if (m_hAudioCaptureThread == NULL) {
			m_bAudioCapturing = FALSE;
			return FALSE;
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
	//video
	if (m_hEvent != NULL && m_hThread != NULL) {
		SetEvent(m_hEvent);
		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hThread);
		m_hThread = NULL;

		CloseHandle(m_hEvent);
		m_hEvent = NULL;
	}

	//audio
	if (m_bAudioCapturing) {
		m_bAudioCapturing = FALSE;
		if (m_hAudioEvent)
			SetEvent(m_hAudioEvent);
		WaitForSingleObject(m_hAudioCaptureThread, INFINITE);
		CloseHandle(m_hAudioCaptureThread);
		m_hAudioCaptureThread = NULL;
	}

	if (m_hChannel) {
		if (MW_SUCCEEDED != MWStopAudioCapture(m_hChannel))
			OutputDebugStringA("CMultiAudioCaptureDlg::Stop audio capture return error !");
	}

	if (m_hAudioEvent) {
		CloseHandle(m_hAudioEvent);
		m_hAudioEvent = NULL;
	}

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}
}

DWORD CPreviewThread::VideoCaptureThreadProcEx()
{
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hTimerEvent		= CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD cbStride = FOURCC_CalcMinStride(m_dwFOURCC, m_cx, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(m_dwFOURCC, m_cx, m_cy, cbStride);
	BYTE* byBuffer = new BYTE[dwFrameSize];
	if (m_pCallback != NULL) {
		memset(byBuffer, 0, dwFrameSize);
		m_pCallback->OnVideoCallback(byBuffer, cbStride, FALSE);
	}

	MW_RESULT xr;
	do {
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

		HNOTIFY hSignalNotify = MWRegisterNotify(m_hChannel, hNotifyEvent, MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE);
		if (hSignalNotify == NULL)
			break;

		HTIMER hTimer = MWRegisterTimer(m_hChannel, hTimerEvent);
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

			HANDLE aEvent[] = {m_hEvent, hTimerEvent, hNotifyEvent};
			DWORD dwRet = WaitForMultipleObjects(3, aEvent, FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED)
				break;
			else if(dwRet == WAIT_OBJECT_0 + 1)
			{
				xr = MWGetVideoSignalStatus(m_hChannel, &m_status);
				if (xr != MW_SUCCEEDED)
					break;

				xr = MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				xr = MWGetVideoFrameInfo(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, &videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				xr = MWCaptureVideoFrameToVirtualAddressEx(
					m_hChannel,
					MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED,
					byBuffer,
					dwFrameSize,
					cbStride,
					FALSE,
					NULL,
					m_dwFOURCC,
					m_cx,
					m_cy,
					0,
					0,									//partical notify
					NULL,								//OSD
					NULL,								//OSD rect
					0,
					100,
					0,
					100,
					0,
					MWCAP_VIDEO_DEINTERLACE_WEAVE,								//deinterlace mode
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
				xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);

				if (m_pCallback != NULL) {
					BOOL bSignalValid = (m_status.state == MWCAP_VIDEO_SIGNAL_LOCKED) ? TRUE : FALSE;
					m_pCallback->OnVideoCallback(byBuffer, cbStride, bSignalValid);
				}

				MWCAP_VIDEO_CAPTURE_STATUS captureStatus;
				xr = MWGetVideoCaptureStatus(m_hChannel, &captureStatus);
			}
			else if(dwRet == WAIT_OBJECT_0 + 2)
			{
				MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
				xr = MWGetVideoSignalStatus(m_hChannel, &videoSignalStatus);
				if (xr != MW_SUCCEEDED)
					continue;

				if (m_pCallback != NULL) {
					BOOL bSignalValid = (m_status.state == MWCAP_VIDEO_SIGNAL_LOCKED) ? TRUE : FALSE;
					m_pCallback->OnVideoSignalChanged(videoSignalStatus.cx, videoSignalStatus.cy, bSignalValid);
				}
			}
		}

		xr = MWUnregisterNotify(m_hChannel, hSignalNotify);
		xr = MWUnregisterTimer(m_hChannel, hTimer);
		xr = MWStopVideoCapture(m_hChannel);

	} while (FALSE);

	if (m_pCallback != NULL) {
		memset(byBuffer, 0, dwFrameSize);
		m_pCallback->OnVideoCallback(byBuffer, cbStride, FALSE);
	}

	delete [] byBuffer;

	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);

	return 0;
}

DWORD CPreviewThread::AudioCaptureThreadProcEx() {	

	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HNOTIFY hAudioNotify = MWRegisterNotify(m_hChannel, hNotifyEvent, MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED | MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE);

	ULONGLONG ullStatusBits = 0LL;

	MWCAP_AUDIO_SIGNAL_STATUS audioSignalStatus;

	int nAudioSampleCount = 0;
	while (m_bAudioCapturing) {
		MWGetAudioSignalStatus(m_hChannel, &audioSignalStatus);

		HANDLE aEvent[] = {m_hAudioEvent, hNotifyEvent};
		WaitForSingleObject(hNotifyEvent, INFINITE);
		DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE, INFINITE);
		if(dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED)
			break;

		if (MW_SUCCEEDED != MWGetNotifyStatus(m_hChannel, hAudioNotify, &ullStatusBits))
			continue;
		if(ullStatusBits & MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE)
		{
			MW_RESULT xr;
			DWORD dwInputCount = 0;
			xr = MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount);

			MWCAP_AUDIO_SIGNAL_STATUS audioSignalStatus;
			xr = MWGetAudioSignalStatus(m_hChannel, &audioSignalStatus);
			if (m_pCallback != NULL) {
				m_bAudioValid		= (dwInputCount > 0 && ((audioSignalStatus.wChannelValid & 1) != 0));
				m_bIsLPCM			= audioSignalStatus.bLPCM;
				m_cBitsPerSample	= audioSignalStatus.cBitsPerSample;
				m_dwSampleRate		= audioSignalStatus.dwSampleRate;

				BOOL bSignalValid = (audioSignalStatus.bChannelStatusValid == 1) ? TRUE : FALSE;
				m_pCallback->OnAudioSignalChanged(m_bAudioValid, m_bIsLPCM, m_cBitsPerSample, m_dwSampleRate, bSignalValid);
				//break;
			}
		}
		else if (ullStatusBits & MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED)
		{
			do {
				if (MW_SUCCEEDED != MWCaptureAudioFrame(m_hChannel, &m_audioFrame))
					break;

				if (m_bIsLPCM)
				{
					if(m_bPreviewFlag == FALSE)
					{
						for (int i = 0; i < MWCAP_AUDIO_SAMPLES_PER_FRAME; i++)
						{
							for(int j = 0; j < m_nPresentChannel / 2; j++)
							{
								short sLeft = (short)(m_audioFrame.adwSamples[i * 8 + j] >> 16);
								short sRight = (short)(m_audioFrame.adwSamples[i * 8 + 4 + j] >> 16);
								m_byAudioSamples[i * 8 +j] = (short)sLeft;
								m_byAudioSamples[i * 8 + 1 + j] = (short)sRight;
							}
						}
						if (m_pCallback != NULL) {
							BOOL bSignalValid = (audioSignalStatus.bChannelStatusValid == 1) ? TRUE : FALSE;
							m_pCallback->OnAudioCallback((const BYTE*)m_byAudioSamples, MWCAP_AUDIO_SAMPLES_PER_FRAME * 8 * sizeof(short), audioSignalStatus.dwSampleRate, bSignalValid);
						}
					}
					else
					{
						for (int i = 0; i < MWCAP_AUDIO_SAMPLES_PER_FRAME; i++)
						{
							short sLeft = (short)(m_audioFrame.adwSamples[i * 8] >> 16);
							short sRight = (short)(m_audioFrame.adwSamples[i * 8 + 4] >> 16);
							m_byAudioSamplesPreview[i * 2] = sLeft;
							m_byAudioSamplesPreview[i * 2 + 1] = sRight;
						}
						if (m_pCallback != NULL) {
							BOOL bSignalValid = (audioSignalStatus.wChannelValid !=0 ) ? TRUE : FALSE;
							m_pCallback->OnAudioCallback((const BYTE*)m_byAudioSamplesPreview, MWCAP_AUDIO_SAMPLES_PER_FRAME * 2 * sizeof(short), audioSignalStatus.dwSampleRate, bSignalValid);
						}
					}
				}
			} while (TRUE);
		}
	}
	MWUnregisterNotify(m_hChannel, hAudioNotify);

	return 0;
};