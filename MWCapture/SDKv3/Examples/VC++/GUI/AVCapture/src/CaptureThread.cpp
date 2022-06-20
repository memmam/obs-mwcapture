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

#include "vector"
using namespace std;

RECT SetRect(int x, int y, int cx, int cy)
{
	RECT ret;
	ret.left = x;
	ret.top = y;
	ret.right = cx;
	ret.bottom = cy;
	return ret;
}

CCaptureThread::CCaptureThread()
{
	m_hVideoThread = NULL;
	m_hAudioThread = NULL;
	m_hExitVideoThread = NULL;
	m_hExitAudioThread = NULL;

	m_hChannel = NULL;

	m_dFramePerSecond = 0.0f;
	m_dSamplePerSecond = 0.0f;

	m_pCallback = NULL;

	m_llVideoFrame = 0LL;
	m_llAudioSample = 0LL;

	m_bRunning = FALSE;
	m_nClip = CLIP_NONE;

	m_p_buf = NULL;
}

CCaptureThread::~CCaptureThread() {}

BOOL CCaptureThread::Create(MWCAP_VIDEO_COLOR_FORMAT colorFormat,
			    MWCAP_VIDEO_QUANTIZATION_RANGE quantRange,
			    int nIndex, int cx, int cy, DWORD dwFourcc,
			    DWORD dwFrameDuration, ICaptureCallback *pCallback,
			    BOOL bCaptureAudio, DWORD dwParam, int nClip)
{
	TCHAR szDevicePath[MAX_PATH];
	MWGetDevicePath(g_nValidChannel[nIndex], szDevicePath);

	BOOL bRet = FALSE;
	do {
		m_hChannel = MWOpenChannelByPath(szDevicePath);
		if (m_hChannel == NULL)
			break;

		DWORD dwInputCount = 0;
		if (MW_SUCCEEDED !=
		    MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount))
			break;

		MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
		if (MW_SUCCEEDED !=
		    MWGetVideoSignalStatus(m_hChannel, &videoSignalStatus))
			break;

		MWCAP_AUDIO_SIGNAL_STATUS audioSignalStatus;
		if (MW_SUCCEEDED !=
		    MWGetAudioSignalStatus(m_hChannel, &audioSignalStatus))
			break;

		m_bVideoValid =
			(videoSignalStatus.state == MWCAP_VIDEO_SIGNAL_LOCKED);
		if (m_bVideoValid) {
			m_cx = videoSignalStatus.cx;
			m_cy = videoSignalStatus.cy;
			m_colorFormat = videoSignalStatus.colorFormat;
			m_quantRange = videoSignalStatus.quantRange;
			m_dwFrameDuration = videoSignalStatus.dwFrameDuration;
		} else {
			MWCAP_VIDEO_CAPS t_video_caps;
			MWGetVideoCaps(m_hChannel, &t_video_caps);
			m_cx = cx > t_video_caps.wMaxOutputWidth
				       ? t_video_caps.wMaxOutputWidth
				       : cx;
			m_cy = cy > t_video_caps.wMaxOutputHeight
				       ? t_video_caps.wMaxOutputHeight
				       : cy;
			m_colorFormat = colorFormat;
			m_quantRange = quantRange;
			m_dwFrameDuration = dwFrameDuration;
		}

		int t_n_stride = FOURCC_CalcMinStride(dwFourcc, m_cx, 2);
		int t_n_size =
			FOURCC_CalcImageSize(dwFourcc, m_cx, m_cy, t_n_stride);
		if (t_n_size > 4120 * 2160 * 2) {
			m_cx = 4120;
			m_cy = 2160;
		}

		m_bAudioValid = (dwInputCount > 0 &&
				 ((audioSignalStatus.wChannelValid & 1) != 0));
		m_bLPCM = audioSignalStatus.bLPCM;
		m_cBitsPerSample = audioSignalStatus.cBitsPerSample;
		m_dwSampleRate = audioSignalStatus.dwSampleRate;

		m_pCallback = pCallback;
		m_dwParam = dwParam;
		m_nClip = nClip;
		m_dwFourcc = dwFourcc;

		m_hExitVideoThread = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hExitVideoThread == NULL)
			break;

		m_bRunning = TRUE;

		m_hVideoThread = CreateThread(NULL, 0, VideoThreadProc,
					      (LPVOID)this, 0, NULL);
		if (m_hVideoThread == NULL)
			break;

		if (bCaptureAudio) {
			m_hExitAudioThread =
				CreateEvent(NULL, FALSE, FALSE, NULL);
			if (m_hExitAudioThread == NULL)
				break;
			m_hAudioThread = CreateThread(NULL, 0, AudioThreadProc,
						      (LPVOID)this, 0, NULL);
			if (m_hAudioThread == NULL)
				break;
		}

		bRet = TRUE;
	} while (FALSE);

	if (!bRet) {
		Destroy();
	}
	return TRUE;
}

void CCaptureThread::Destroy()
{
	m_bRunning = FALSE;

	if (m_hExitVideoThread != NULL && m_hVideoThread != NULL) {
		SetEvent(m_hExitVideoThread);
		WaitForSingleObject(m_hVideoThread, INFINITE);

		CloseHandle(m_hExitVideoThread);
		m_hExitVideoThread = NULL;

		CloseHandle(m_hVideoThread);
		m_hVideoThread = NULL;
	}
	if (m_hExitAudioThread != NULL && m_hAudioThread != NULL) {
		SetEvent(m_hExitAudioThread);
		WaitForSingleObject(m_hAudioThread, INFINITE);

		CloseHandle(m_hExitAudioThread);
		m_hExitAudioThread = NULL;

		CloseHandle(m_hAudioThread);
		m_hAudioThread = NULL;
	}

	if (m_hChannel) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	m_llVideoFrame = 0LL;
	m_llAudioSample = 0LL;
}

DWORD CCaptureThread::GetAdjustFrameDuration()
{
	return m_dwFrameDuration;
}

void CCaptureThread::SetRenderBuffer(CMWRenderBuffer *t_p_buf)
{
	m_p_buf = t_p_buf;
}

DWORD CCaptureThread::VideoProThreadProc()
{
	// Preview
	DWORD cbStride = FOURCC_CalcMinStride(m_dwFourcc, m_cx, 2);
	DWORD dwFrameSize =
		FOURCC_CalcImageSize(m_dwFourcc, m_cx, m_cy, cbStride);

	BYTE *byBuffer = NULL;
	if (m_p_buf == NULL) {
		byBuffer = new BYTE[dwFrameSize];
		memset(byBuffer, 0xFF, dwFrameSize);
	}

	// Wait Events
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hTimerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	MW_RESULT xr;
	do {
		xr = MWStartVideoCapture(m_hChannel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;

		HNOTIFY hSignalNotify =
			MWRegisterNotify(m_hChannel, hNotifyEvent,
					 MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE);
		if (hSignalNotify == NULL)
			break;

		HTIMER hTimerNotify = MWRegisterTimer(m_hChannel, hTimerEvent);
		if (hTimerNotify == NULL)
			break;

		DWORD dwFrameCount = 0;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		LONGLONG llExpireTime = llBegin;
		DWORD dwFrameDuration = m_dwFrameDuration;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llLast = llBegin;
		LONGLONG llNow = 0;
		LONGLONG t_ll_capture = 0;
		LONGLONG t_ll_capture_called = 0;
		LONGLONG t_ll_reset_called = 0;

		vector<BYTE *> t_vec_buffer;

		if (byBuffer != NULL) {
			MWPinVideoBuffer(m_hChannel, byBuffer, dwFrameSize);
			t_vec_buffer.push_back(byBuffer);
		} else {
			int t_n_num = m_p_buf->get_buf_num();
			for (int i = 0; i < t_n_num; i++) {
				mw_buffer_t *t_p_buf =
					m_p_buf->get_buf_by_id(i);
				if (t_p_buf->m_p_buf != NULL) {
					MWPinVideoBuffer(m_hChannel,
							 t_p_buf->m_p_buf,
							 dwFrameSize);
					t_vec_buffer.push_back(
						t_p_buf->m_p_buf);
				}
			}
		}

		while (m_bRunning) {

			MWCAP_VIDEO_SIGNAL_STATUS t_status;
			MWGetVideoSignalStatus(m_hChannel, &t_status);
			if (m_hAudioThread != NULL &&
			    ((dwFrameDuration != m_dwFrameDuration) ||
			     (dwFrameCount % 10 == 0))) {
				dwFrameDuration = GetAdjustFrameDuration();
			}

			xr = MWGetDeviceTime(m_hChannel, &llNow);
			if (xr != MW_SUCCEEDED)
				continue;

			llExpireTime = llExpireTime + dwFrameDuration;

			//llExpireTime = llExpireTime<llNow?llNow+dwFrameDuration:llExpireTime;

			xr = MWScheduleTimer(m_hChannel, hTimerNotify,
					     llExpireTime);
			if (xr != MW_SUCCEEDED) {
				continue;
			}

			HANDLE aEventNotify[3] = {m_hExitVideoThread,
						  hTimerEvent, hNotifyEvent};
			DWORD dwRet = WaitForMultipleObjects(3, aEventNotify,
							     FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED) {
				break;
			} else if (dwRet == WAIT_OBJECT_0 + 1) {

				MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
				if (MW_SUCCEEDED !=
				    MWGetVideoBufferInfo(m_hChannel,
							 &videoBufferInfo))
					continue;

				MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
				xr = MWGetVideoSignalStatus(m_hChannel,
							    &videoSignalStatus);
				if (xr != MW_SUCCEEDED)
					continue;

				MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
				xr = MWGetVideoFrameInfo(
					m_hChannel,
					videoBufferInfo.iNewestBufferedFullFrame,
					&videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				RECT rcSrc;
				switch (m_nClip) {
				case CLIP_NONE:
					rcSrc = SetRect(0, 0,
							videoSignalStatus.cx,
							videoSignalStatus.cy);
					break;
				case CLIP_TOPLEFT:
					rcSrc = SetRect(
						0, 0, videoSignalStatus.cx / 2,
						videoSignalStatus.cy / 2);
					break;
				case CLIP_TOPRIGHT:
					rcSrc = SetRect(
						videoSignalStatus.cx / 2, 0,
						videoSignalStatus.cx,
						videoSignalStatus.cy / 2);
					break;
				case CLIP_BOTTOMLEFT:
					rcSrc = SetRect(
						0, videoSignalStatus.cy / 2,
						videoSignalStatus.cx / 2,
						videoSignalStatus.cy);
					break;
				case CLIP_BOTTOMRIGHT:
					rcSrc = SetRect(
						videoSignalStatus.cx / 2,
						videoSignalStatus.cy / 2,
						videoSignalStatus.cx,
						videoSignalStatus.cy);
					break;
				}

				BYTE *t_p_data = byBuffer;
				mw_buffer_t *t_p_mwbuf = NULL;
				if (m_p_buf != NULL) {
					t_p_mwbuf = m_p_buf->get_buf_to_fill();
					if (t_p_mwbuf)
						t_p_data = t_p_mwbuf->m_p_buf;
				}

				xr = MWCaptureVideoFrameToVirtualAddressEx(
					m_hChannel,
					MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED,
					t_p_data, dwFrameSize, cbStride, FALSE,
					NULL, m_dwFourcc, m_cx, m_cy, 0, 0,
					NULL, NULL, 0, 100, 0, 100, 0,
					MWCAP_VIDEO_DEINTERLACE_BLEND,
					MWCAP_VIDEO_ASPECT_RATIO_IGNORE, &rcSrc,
					NULL, 0, 0,
					MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
					MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
					MWCAP_VIDEO_SATURATION_UNKNOWN);

				WaitForSingleObject(hCaptureEvent, INFINITE);

				xr = MWGetVideoCaptureStatus(m_hChannel,
							     &captureStatus);

				LONGLONG llCurrent = 0LL;
				MWGetDeviceTime(m_hChannel, &llCurrent);

				if (t_p_mwbuf) {
					t_p_mwbuf->m_u64_timestamp =
						llCurrent / 10000;
					m_p_buf->put_buf_filled();
				}

				if (m_pCallback != NULL) {
					m_pCallback->OnCaptureCallback(
						t_p_data, cbStride, m_dwParam);
				}

				dwFrameCount++;
				m_llVideoFrame++;

				if (dwFrameCount % 10 == 0) {
					m_dFramePerSecond =
						(double)dwFrameCount *
						10000000LL /
						(llCurrent - llLast);
					if (llCurrent - llLast > 30000000LL) {
						llLast = llCurrent;
						dwFrameCount = 0;
					}
				}
			} else if (dwRet == WAIT_OBJECT_0 + 2) {
				MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
				xr = MWGetVideoSignalStatus(m_hChannel,
							    &videoSignalStatus);
				if (xr != MW_SUCCEEDED)
					continue;

				if (m_pCallback != NULL) {
					MWUnpinVideoBuffer(m_hChannel,
							   byBuffer);
					m_pCallback->OnVideoSignalChanged(
						videoSignalStatus.cx,
						videoSignalStatus.cy,
						m_dwParam);
				}
			}
		}

		while (!t_vec_buffer.empty()) {
			MWUnpinVideoBuffer(m_hChannel, t_vec_buffer.back());
			t_vec_buffer.pop_back();
		}

		xr = MWUnregisterNotify(m_hChannel, hSignalNotify);
		xr = MWUnregisterTimer(m_hChannel, hTimerNotify);
		xr = MWStopVideoCapture(m_hChannel);

	} while (FALSE);

	CloseHandle(hCaptureEvent);
	CloseHandle(hTimerEvent);
	CloseHandle(hNotifyEvent);

	if (byBuffer != NULL) {
		delete[] byBuffer;
	}

	return 0;
}

DWORD CCaptureThread::VideoEcoThreadProc()
{
	//create event and notify
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HNOTIFY hSignalNotify = MWRegisterNotify(
		m_hChannel, hNotifyEvent, MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE);

	MW_RESULT xr = MW_FAILED;

	//start video eco capture
	MWCAP_VIDEO_SIGNAL_STATUS t_video_signal_status;
	xr = MWGetVideoSignalStatus(m_hChannel, &t_video_signal_status);
	MWCAP_VIDEO_ECO_CAPTURE_OPEN ecoCaptureOpen = {
		hCaptureEvent, m_dwFourcc, m_cx, m_cy, -1};
	if (t_video_signal_status.state != MWCAP_VIDEO_SIGNAL_LOCKED)
		ecoCaptureOpen.llFrameDuration = 400000;

	xr = MWStartVideoEcoCapture(m_hChannel, &ecoCaptureOpen);

	//set video capture frame
	MWCAP_VIDEO_ECO_CAPTURE_FRAME videoFrame[4] = {0};
	int t_n_num = 4;
	int iCompleted = 0;
	DWORD cbStride = FOURCC_CalcMinStride(m_dwFourcc, m_cx, 2);
	DWORD dwFrameSize =
		FOURCC_CalcImageSize(m_dwFourcc, m_cx, m_cy, cbStride);

	for (int i = 0; i < t_n_num; i++) {
		videoFrame[i].pvFrame = (MWCAP_PTR64)malloc(dwFrameSize);
		videoFrame[i].cbFrame = dwFrameSize;
		videoFrame[i].cbStride = cbStride;
		videoFrame[i].pvContext = &(videoFrame[i]);
		xr = MWCaptureSetVideoEcoFrame(m_hChannel, &(videoFrame[i]));
	}

	LONGLONG llBegin = 0LL;
	xr = MWGetDeviceTime(m_hChannel, &llBegin);
	LONGLONG llLast = llBegin;
	DWORD dwFrameCount = 0;

	//video capture loop
	while (m_bRunning) {
		HANDLE aEventNotify[] = {m_hExitVideoThread, hCaptureEvent,
					 hNotifyEvent};
		DWORD dwRet = WaitForMultipleObjects(
			sizeof(aEventNotify) / sizeof(aEventNotify[0]),
			aEventNotify, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED) {
			break;
		} else if (dwRet == WAIT_OBJECT_0 + 1) {
			MWCAP_VIDEO_ECO_CAPTURE_STATUS captureStatus;
			while (m_bRunning) {
				xr = MWGetVideoEcoCaptureStatus(m_hChannel,
								&captureStatus);
				if (xr == MW_FAILED ||
				    captureStatus.pvFrame == NULL) {
					break;
				}

				LONGLONG llCurrent = 0LL;
				MWGetDeviceTime(m_hChannel, &llCurrent);

				if (m_p_buf) {
					BYTE *t_p_data = NULL;
					mw_buffer_t *t_p_mwbuf = NULL;
					t_p_mwbuf = m_p_buf->get_buf_to_fill();
					if (t_p_mwbuf) {
						t_p_data = t_p_mwbuf->m_p_buf;
						memcpy(t_p_data,
						       (BYTE *)captureStatus
							       .pvFrame,
						       dwFrameSize);
						t_p_mwbuf->m_u64_timestamp =
							llCurrent / 10000;
						m_p_buf->put_buf_filled();
					}
				}

				if (m_pCallback != NULL) {
					m_pCallback->OnCaptureCallback(
						(BYTE *)captureStatus.pvFrame,
						cbStride, m_dwParam);
				}

				xr = MWCaptureSetVideoEcoFrame(
					m_hChannel,
					(MWCAP_VIDEO_ECO_CAPTURE_FRAME *)
						videoFrame[iCompleted]
							.pvContext);
				iCompleted = (iCompleted + 1) %
					     (sizeof(videoFrame) /
					      sizeof(videoFrame[0]));

				dwFrameCount++;

				if (dwFrameCount % 10 == 0) {
					m_dFramePerSecond =
						(double)dwFrameCount *
						10000000LL /
						(llCurrent - llLast);

					if (llCurrent - llLast > 30000000LL) {
						llLast = llCurrent;
						dwFrameCount = 0;
					}
				}
			}
		} else if (dwRet == WAIT_OBJECT_0 + 2) {
			MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
			xr = MWGetVideoSignalStatus(m_hChannel,
						    &videoSignalStatus);
			if (xr != MW_SUCCEEDED)
				continue;

			if (m_pCallback != NULL) {
				m_pCallback->OnVideoSignalChanged(
					videoSignalStatus.cx,
					videoSignalStatus.cy, m_dwParam);
			}
		}
	}

	//cleanup
	if (m_hChannel) {
		MWStopVideoEcoCapture(m_hChannel);
		m_hChannel = NULL;
	}

	if (hCaptureEvent) {
		CloseHandle(hCaptureEvent);
		hCaptureEvent = NULL;
	}

	if (hSignalNotify) {
		MWUnregisterNotify(m_hChannel, hSignalNotify);
		hSignalNotify = NULL;
	}

	if (hNotifyEvent) {
		CloseHandle(hNotifyEvent);
		hNotifyEvent = NULL;
	}

	for (int i = 0; i < sizeof(videoFrame) / sizeof(videoFrame[0]); i++) {
		if (videoFrame[i].pvFrame) {
			free(videoFrame[i].pvFrame);
			videoFrame[i].pvFrame = NULL;
		}
	}

	return 0;
}

DWORD CCaptureThread::AudioThreadProc()
{
	HANDLE hAudioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HNOTIFY hAudioNotify = NULL;

	MW_RESULT xr;
	do {
		xr = MWStartAudioCapture(m_hChannel);
		if (xr != MW_SUCCEEDED)
			break;

		hAudioNotify = MWRegisterNotify(
			m_hChannel, hAudioEvent,
			MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE |
				MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED);
		if (hAudioNotify == NULL)
			break;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		DWORD dwSampleCount = 0;
		LONGLONG llLast = llBegin;

		while (m_bRunning) {
			HANDLE aEvent[] = {m_hExitAudioThread, hAudioEvent};
			DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE,
							     INFINITE);
			if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED)
				break;
			else if (dwRet == WAIT_OBJECT_0 + 1) {
				ULONGLONG ullStatusBits = 0LL;
				MWGetNotifyStatus(m_hChannel, hAudioNotify,
						  &ullStatusBits);

				if (ullStatusBits &
				    MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE) {
					DWORD dwInputCount = 0;
					xr = MWGetAudioInputSourceArray(
						m_hChannel, NULL,
						&dwInputCount);

					MWCAP_AUDIO_SIGNAL_STATUS
					audioSignalStatus;
					xr = MWGetAudioSignalStatus(
						m_hChannel, &audioSignalStatus);
					if (m_pCallback != NULL) {
						m_bAudioValid =
							(dwInputCount > 0 &&
							 ((audioSignalStatus
								   .wChannelValid &
							   1) != 0));
						m_bLPCM =
							audioSignalStatus.bLPCM;
						m_cBitsPerSample =
							audioSignalStatus
								.cBitsPerSample;
						m_dwSampleRate =
							audioSignalStatus
								.dwSampleRate;

						m_pCallback->OnAudioSignalChanged(
							m_bAudioValid, m_bLPCM,
							m_cBitsPerSample,
							m_dwSampleRate,
							m_dwParam);
					}
				}

				if (ullStatusBits &
				    MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED) {
					do {
						MWCAP_AUDIO_CAPTURE_FRAME
						audioFrame;
						xr = MWCaptureAudioFrame(
							m_hChannel,
							&audioFrame);

						if (xr == MW_SUCCEEDED) {
							short asAudioSamples
								[MWCAP_AUDIO_SAMPLES_PER_FRAME *
								 2];
							for (int i = 0;
							     i <
							     MWCAP_AUDIO_SAMPLES_PER_FRAME;
							     i++) {
								short sLeft =
									(short)(audioFrame
											.adwSamples
												[i *
												 8] >>
										16);
								short sRight =
									(short)(audioFrame
											.adwSamples
												[i * 8 +
												 4] >>
										16);

								asAudioSamples[i *
									       2] =
									sLeft;
								asAudioSamples
									[i * 2 +
									 1] = sRight;
							}

							LONGLONG llCurrent =
								0LL;
							MWGetDeviceTime(
								m_hChannel,
								&llCurrent);

							if (m_pCallback !=
							    NULL) {
								m_pCallback->OnAudioCallback(
									(const BYTE
										 *)
										asAudioSamples,
									MWCAP_AUDIO_SAMPLES_PER_FRAME *
										2 *
										sizeof(short),
									llCurrent /
										10000,
									m_dwParam);
							}

							dwSampleCount +=
								MWCAP_AUDIO_SAMPLES_PER_FRAME;
							m_llAudioSample +=
								MWCAP_AUDIO_SAMPLES_PER_FRAME;

							if (dwSampleCount %
								    (100 *
								     MWCAP_AUDIO_SAMPLES_PER_FRAME) ==
							    0) {
								m_dSamplePerSecond =
									(double)dwSampleCount *
									10000000LL /
									(llCurrent -
									 llLast);

								if (llCurrent -
									    llLast >
								    30000000LL) {
									llLast =
										llCurrent;
									dwSampleCount =
										0;
								}
							}
						}
					} while (xr == MW_SUCCEEDED &&
						 m_bRunning);
				}
			}
		}

		xr = MWUnregisterNotify(m_hChannel, hAudioNotify);
		xr = MWStopAudioCapture(m_hChannel);
	} while (FALSE);

	CloseHandle(hAudioEvent);

	return 0;
}

int CCaptureThread::GetChannelIndex(int nIndex)
{
	if (nIndex > 0 && nIndex < g_nValidChannelCount)
		return g_nValidChannel[nIndex];
	else
		return -1;
}