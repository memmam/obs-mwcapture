/************************************************************************************************/
// VideoWnd.cpp : implementation of the CVideoWnd class

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
#include "VideoWnd.h"

#define WM_VIDEOSIGNAL_CHANGED (WM_USER + 301)
// CVideoWnd
IMPLEMENT_DYNAMIC(CVideoWnd, CWnd)
CVideoWnd::CVideoWnd(void)
{
	m_pNDI_send = NULL;
	m_pRenderer = NULL;
	m_pAudioRenderer = NULL;

	m_nChannelIndex = 0;
	m_cx = PREVIEW_WIDTH;
	m_cy = PREVIEW_HEIGHT;
	m_dwFrameDuration = 333333;
	m_bNDISend = FALSE;

	m_n_samples = 192 * 6;
	memset(m_cs_audio_0, 0, m_n_samples * 4);
	memset(m_cs_audio_1, 0, m_n_samples * 4);
	m_n_index_audio = 0;
	m_n_pos = 0;

	m_hEventExitVideo = NULL;
	m_hEventSendVideo = NULL;
	m_hNDIThreadVideo = NULL;

	m_hEventExitAudio = NULL;
	m_hEventSendAudio = NULL;
	m_hNDIThreadAudio = NULL;

	m_n_num_data_video = 3;
	m_n_size_data_video = 1920 * 1080 * 4;
	m_pa_data_video = new char *[m_n_num_data_video];
	for (int i = 0; i < m_n_num_data_video; i++) {
		m_pa_data_video[i] = new char[m_n_size_data_video];
		m_queue_used_video.push(m_pa_data_video[i]);
	}

	InitializeCriticalSection(&m_cs_send_video);
}

CVideoWnd::~CVideoWnd(void)
{
	if (m_pa_data_video != NULL) {
		for (int i = 0; i < m_n_num_data_video; i++) {
			if (m_pa_data_video[i] != NULL) {
				delete[] m_pa_data_video[i];
				m_pa_data_video[i] = NULL;
			}
		}
		delete[] m_pa_data_video;
		m_pa_data_video = NULL;
	}

	DeleteCriticalSection(&m_cs_send_video);
}

BEGIN_MESSAGE_MAP(CVideoWnd, CWnd)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()

ON_MESSAGE(WM_VIDEOSIGNAL_CHANGED, &CVideoWnd::OnMsgVideoSignalChanged)
END_MESSAGE_MAP()

int CVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//video
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CVideoWnd::OnDestroy()
{
	CWnd::OnDestroy();

	m_pNDI_send = NULL;
}

BOOL CVideoWnd::OnEraseBkgnd(CDC *pDC)
{

	return CWnd::OnEraseBkgnd(pDC);
}

void CVideoWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL CVideoWnd::OpenChannel(int nIndex, int cx, int cy, DWORD dwFrameDuration,
			    DWORD dwFOURCC)
{
	NDIlib_send_instance_t t_pNDI_Send = m_pNDI_send;
	CloseChannel();
	m_pNDI_send = t_pNDI_Send;

	// reset buffer
	memset(m_cs_audio_0, 0, 7680);
	m_n_pos = 0;

	while (!m_queue_send_video.empty())
		m_queue_send_video.pop();

	while (!m_queue_used_video.empty())
		m_queue_used_video.pop();

	for (int i = 0; i < m_n_num_data_video; i++)
		m_queue_used_video.push(m_pa_data_video[i]);

	m_nChannelIndex = g_nValidChannel[nIndex];
	m_cx = cx;
	m_cy = cy;
	m_dwFourcc = dwFOURCC;
	m_dwFrameDuration = dwFrameDuration;

	BOOL bRet = FALSE;
	bRet = m_thread.Create(m_nChannelIndex, m_cx, m_cy, m_dwFrameDuration,
			       dwFOURCC, TRUE, this);

	MWCAP_CHANNEL_INFO videoInfo = {0};
	if (MW_SUCCEEDED != MWGetChannelInfoByIndex(nIndex, &videoInfo)) {
		return FALSE;
	}

	// Open channel
	TCHAR szDevicePath[MAX_PATH];
	if (MW_SUCCEEDED != MWGetDevicePath(nIndex, szDevicePath)) {
		OutputDebugStringA("ERROR: MWGetDevicePath fail !\n");
		return FALSE;
	}
	HCHANNEL hChannel = MWOpenChannelByPath(szDevicePath);
	if (hChannel == NULL) {
		return FALSE;
	}

	MWCAP_VIDEO_SIGNAL_STATUS t_status;
	MWGetVideoSignalStatus(hChannel, &t_status);
	MWCAP_VIDEO_COLOR_FORMAT t_colorFormat =
		MWCAP_VIDEO_COLOR_FORMAT_YUV709;
	MWCAP_VIDEO_QUANTIZATION_RANGE t_quantRange =
		MWCAP_VIDEO_QUANTIZATION_LIMITED;
	if (t_status.state == MWCAP_VIDEO_SIGNAL_LOCKED) {
		t_colorFormat = t_status.colorFormat;
		t_quantRange = t_status.quantRange;
	}
	mw_color_space_e color_format;
	mw_color_space_level_e input_range;
	mw_color_space_level_e output_range = MW_CSP_LEVELS_PC;
	if (MWCAP_VIDEO_COLOR_FORMAT_YUV601 == t_colorFormat)
		color_format = MW_CSP_BT_601;
	else if (MWCAP_VIDEO_COLOR_FORMAT_YUV709 == t_colorFormat)
		color_format = MW_CSP_BT_709;
	else if (MWCAP_VIDEO_COLOR_FORMAT_YUV2020 == t_colorFormat)
		color_format = MW_CSP_BT_2020;
	else
		color_format = MW_CSP_BT_709;

	if (MWCAP_VIDEO_QUANTIZATION_FULL == t_quantRange)
		input_range = MW_CSP_LEVELS_PC;
	else if (MWCAP_VIDEO_QUANTIZATION_LIMITED == t_quantRange)
		input_range = MW_CSP_LEVELS_TV;
	else
		input_range = MW_CSP_LEVELS_TV;

	//init render
	CMWAutoLock lock(m_lockVideo);
	m_pRenderer = new MWDXRender();
	if (!m_pRenderer->initialize(cx, cy, dwFOURCC, false, GetSafeHwnd(),
				     color_format, input_range, output_range)) {
		delete m_pRenderer;
		m_pRenderer = NULL;
		return FALSE;
	}

	//audio
	BOOL bLPCM = FALSE;
	BYTE cBitsPerSample = 0;
	DWORD dwSampleRate = 0;
	int nChannels = 0;
	if (!m_thread.GetInputAudioFormat(bLPCM, cBitsPerSample,
					  dwSampleRate)) {
		OutputDebugStringA("Get input audio format fail !\n");
		bLPCM = FALSE;
	}

	if (bLPCM) {
		CMWAutoLock lock(m_lockAudio);
		BOOL bRet = FALSE;
		do {
			m_pAudioRenderer = new CDSoundRenderer();
			if (!m_pAudioRenderer)
				break;

			GUID guid = GUID_NULL;
			if (!m_pAudioRenderer->Initialize(&guid))
				break;

			if (!m_pAudioRenderer->Create(
				    dwSampleRate, 2,
				    MWCAP_AUDIO_SAMPLES_PER_FRAME,
				    DEF_AUDIO_JITTER_FRAMES))
				break;

			if (!m_pAudioRenderer->Run())
				break;

			bRet = TRUE;
		} while (FALSE);

		if (!bRet) {
			m_pAudioRenderer->Deinitialize();
			delete m_pAudioRenderer;
			m_pAudioRenderer = NULL;
		}
	}

	return bRet;
}

void CVideoWnd::CloseChannel()
{
	m_pNDI_send = NULL;
	m_thread.Destroy();
	if (m_pRenderer != NULL) {
		m_pRenderer->cleanup_device();
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	if (m_pAudioRenderer) {
		m_pAudioRenderer->Deinitialize();
		delete m_pAudioRenderer;
		m_pAudioRenderer = NULL;
	}
}

BOOL CVideoWnd::OnVideoCallback(const BYTE *lpData, DWORD cbStride,
				BOOL bSignalValid)
{
	if (m_bNDISend == TRUE && m_pNDI_send != NULL) {
		EnterCriticalSection(&m_cs_send_video);
		if (!m_queue_used_video.empty()) {
			char *t_p_send = m_queue_used_video.front();
			m_queue_used_video.pop();
			if (t_p_send != NULL)
				memcpy(t_p_send, lpData,
				       cbStride * m_videoFrame.yres);
			m_queue_send_video.push(t_p_send);
			if (m_hEventSendVideo != NULL)
				SetEvent(m_hEventSendVideo);
		}
		LeaveCriticalSection(&m_cs_send_video);
	}

	CMWAutoLock lock(m_lockVideo);
	if (m_pRenderer != NULL) {
		m_pRenderer->paint((unsigned char *)lpData);
	}
	return TRUE;
}

BOOL CVideoWnd::OnAudioCallback(const BYTE *lpData, int cbSize,
				DWORD dwSamplerate, BOOL bSignalValid)
{
	CMWAutoLock lock(m_lockAudio);
	if (m_bNDISend == TRUE && m_pNDI_send != NULL) {
		if (bSignalValid == TRUE &&
		    dwSamplerate == m_audioFrame.sample_rate) {
			char *t_p_audio_data = m_n_index_audio == 0
						       ? m_cs_audio_0
						       : m_cs_audio_1;
			memcpy(t_p_audio_data + m_n_pos, lpData, cbSize);
			m_n_pos += cbSize;
			if (m_n_pos >= m_n_samples * 4) {
				m_audioFrame.p_data = (short *)t_p_audio_data;
				m_audioFrame.no_samples = m_n_samples;
				//NDIlib_util_send_send_audio_interleaved_16s(m_pNDI_send, &m_audioFrame);
				if (m_hEventSendAudio != NULL)
					SetEvent(m_hEventSendAudio);
				m_n_pos = 0;
				m_n_index_audio = !m_n_index_audio;
			}
		}
	}
	if (m_pAudioRenderer) {
		m_pAudioRenderer->PutFrame(lpData, cbSize);
	}

	return TRUE;
}

void CVideoWnd::OnVideoSignalChanged(int cx, int cy, BOOL bSignalValid) {}

void CVideoWnd::OnAudioSignalChanged(BOOL bValid, BOOL bLPCM,
				     BYTE cBitsPerSample, DWORD dwSampleRate,
				     BOOL bSignalValid)
{
	CMWAutoLock lock(m_lockAudio);
	PostMessage(WM_VIDEOSIGNAL_CHANGED);
}

DWORD CVideoWnd::NDISendThreadVideoProc()
{
	HANDLE t_arrEvents[] = {m_hEventExitVideo, m_hEventSendVideo};
	m_b_running_video = true;
	while (m_b_running_video) {
		DWORD t_ret =
			WaitForMultipleObjects(2, t_arrEvents, FALSE, INFINITE);
		if (t_ret == WAIT_OBJECT_0)
			break;
		else if (t_ret == WAIT_OBJECT_0 + 1) {
			char *t_p_send = NULL;
			bool t_b_empty = false;
			EnterCriticalSection(&m_cs_send_video);
			if (!m_queue_send_video.empty()) {
				t_p_send = m_queue_send_video.front();
			} else
				t_b_empty = true;
			LeaveCriticalSection(&m_cs_send_video);
			if (t_p_send != NULL) {
				m_videoFrame.p_data = (uint8_t *)t_p_send;
				NDIlib_send_send_video_v2(m_pNDI_send,
							  &m_videoFrame);
			}
			if (!t_b_empty) {
				EnterCriticalSection(&m_cs_send_video);
				m_queue_send_video.pop();
				m_queue_used_video.push(t_p_send);
				LeaveCriticalSection(&m_cs_send_video);
			}
		} else
			break;
	}
	m_b_running_video = false;
	return 0;
}

bool CVideoWnd::CreateNDISendThread()
{
	if (m_hNDIThreadVideo != NULL) {
		DestoryNDISendThread();
		m_hNDIThreadVideo = NULL;
	}

	if (m_hEventExitVideo != NULL) {
		CloseHandle(m_hEventExitVideo);
		m_hEventExitVideo = NULL;
	}

	if (m_hEventSendVideo != NULL) {
		CloseHandle(m_hEventSendVideo);
		m_hEventSendVideo = NULL;
	}

	if (m_hEventExitAudio != NULL) {
		CloseHandle(m_hEventExitAudio);
		m_hEventExitAudio = NULL;
	}

	if (m_hEventSendAudio != NULL) {
		CloseHandle(m_hEventSendAudio);
		m_hEventSendAudio = NULL;
	}

	m_hEventExitVideo = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEventExitVideo == NULL)
		return false;
	m_hEventSendVideo = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEventSendVideo == NULL) {
		CloseHandle(m_hEventExitVideo);
		m_hEventExitVideo = NULL;
		return false;
	}

	// reset buffer
	memset(m_cs_audio_0, 0, 7680);
	m_n_pos = 0;

	while (!m_queue_send_video.empty())
		m_queue_send_video.pop();

	while (!m_queue_used_video.empty())
		m_queue_used_video.pop();

	for (int i = 0; i < m_n_num_data_video; i++)
		m_queue_used_video.push(m_pa_data_video[i]);

	m_hEventExitAudio = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEventExitAudio == NULL)
		return false;
	m_hEventSendAudio = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_hEventSendAudio == NULL) {
		CloseHandle(m_hEventExitAudio);
		m_hEventExitVideo = NULL;
		return false;
	}

	m_hNDIThreadVideo =
		CreateThread(NULL, 0, NDISendThreadVideo, this, 0, NULL);
	if (m_hNDIThreadVideo == NULL) {
		CloseHandle(m_hEventExitVideo);
		m_hEventExitVideo = NULL;
		CloseHandle(m_hEventSendVideo);
		m_hEventSendVideo = NULL;
		return false;
	}

	m_hNDIThreadAudio =
		CreateThread(NULL, 0, NDISendThreadAudio, this, 0, NULL);
	if (m_hNDIThreadAudio == NULL) {
		CloseHandle(m_hEventExitAudio);
		m_hEventExitAudio = NULL;
		CloseHandle(m_hEventSendAudio);
		m_hEventSendAudio = NULL;
		return false;
	}

	SetThreadPriority(m_hNDIThreadAudio, THREAD_PRIORITY_TIME_CRITICAL);

	return true;
}

void CVideoWnd::DestoryNDISendThread()
{
	m_b_running_video = false;
	if (m_hEventExitVideo != NULL)
		SetEvent(m_hEventExitVideo);

	m_b_running_audio = false;
	if (m_hEventExitAudio != NULL)
		SetEvent(m_hEventExitAudio);
}

DWORD CVideoWnd::NDISendThreadAudioProc()
{
	HANDLE t_arrEvents[] = {m_hEventExitAudio, m_hEventSendAudio};
	m_b_running_audio = true;
	while (m_b_running_audio) {
		DWORD t_ret =
			WaitForMultipleObjects(2, t_arrEvents, FALSE, INFINITE);
		if (t_ret == WAIT_OBJECT_0)
			break;
		else if (t_ret == WAIT_OBJECT_0 + 1) {
			NDIlib_util_send_send_audio_interleaved_16s(
				m_pNDI_send, &m_audioFrame);
		} else
			break;
	}
	m_b_running_video = false;
	return 0;
}

LRESULT CVideoWnd::OnMsgVideoSignalChanged(WPARAM w, LPARAM l)
{
	CloseChannel();
	OpenChannel(m_nChannelIndex, m_cx, m_cy, m_dwFrameDuration, m_dwFourcc);
	return 0;
}