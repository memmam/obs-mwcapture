/************************************************************************************************/
// VideoSend.cpp : implementation of the CVideoSend class

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
#include "VideoSend.h"

#define WM_NDIVIDEOSIGNAL_CHANGED (WM_USER + 301)
IMPLEMENT_DYNAMIC(CVideoSend, CWnd)
CVideoSend::CVideoSend(void)
{
	m_nIndex = 0;
	m_pNDI_send = NULL;
	m_nIndex = 0;
	m_cx = 1920;
	m_cy = 1080;
	m_dwFrameDuration = 333334;
	m_dwFourcc = MWFOURCC_BGRA;
}

CVideoSend::~CVideoSend(void) {}

BEGIN_MESSAGE_MAP(CVideoSend, CWnd)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()

ON_MESSAGE(WM_NDIVIDEOSIGNAL_CHANGED, &CVideoSend::OnMsgVideoSignalChanged)
END_MESSAGE_MAP()

int CVideoSend::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//video
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CVideoSend::OnDestroy()
{
	CWnd::OnDestroy();
}

BOOL CVideoSend::OnEraseBkgnd(CDC *pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CVideoSend::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL CVideoSend::CreateNDISender(int nIndex, int cx, int cy,
				 DWORD dwFrameDuration, DWORD dwFourcc,
				 char *pSenderName)
{
	m_nIndex = g_nValidChannel[nIndex];
	m_cx = cx;
	m_cy = cy;
	m_dwFrameDuration = dwFrameDuration;
	m_dwFourcc = dwFourcc;

	CMWAutoLock lock(m_lock);
	// Not required, but "correct" (see the SDK documentation.
	if (!NDIlib_initialize())
		return FALSE;

	// Create an NDI source that is called "My Video" and is clocked to the video.
	NDIlib_send_create_t NDI_send_create_desc;
	NDI_send_create_desc.p_ndi_name = pSenderName;
	NDI_send_create_desc.p_groups = NULL;
	NDI_send_create_desc.clock_video = true;
	NDI_send_create_desc.clock_audio = true;

	// We create the NDI sender
	m_pNDI_send = NDIlib_send_create(&NDI_send_create_desc);
	if (!m_pNDI_send)
		return FALSE;

	SetNDICaptureFrameParams(cx, cy, dwFrameDuration, dwFourcc);

	static const char *p_connection_string =
		"<ndi_product long_name=\"NDILib Send Example.\" "
		"             short_name=\"NDISender\" "
		"             manufacturer=\"(None)\" "
		"             version=\"1.000.000\" "
		"             session=\"default\" "
		"             model_name=\"S1\" "
		"             serial=\"ABCDEFG\"/>";
	NDIlib_metadata_frame_t NDI_connection_type;
	NDI_connection_type.length = strlen(p_connection_string);
	NDI_connection_type.timecode = NDIlib_send_timecode_synthesize;
	NDI_connection_type.p_data = (CHAR *)p_connection_string;

	NDIlib_send_add_connection_metadata(m_pNDI_send, &NDI_connection_type);

	return TRUE;
}

void CVideoSend::DestoryNDISender()
{
	if (m_pNDI_send != NULL) {
		NDIlib_send_destroy(m_pNDI_send);
		m_pNDI_send = NULL;
		// Not required, but you add it will be better
		NDIlib_destroy();
	}
}

BOOL CVideoSend::SetNDICaptureFrameParams(int cx, int cy, DWORD dwFrameDuration,
					  DWORD dwFourcc)
{
	//video
	DWORD cbStride = FOURCC_CalcMinStride(dwFourcc, cx, 2);

	m_videoFrame.xres = cx;
	m_videoFrame.yres = cy;
	m_videoFrame.frame_rate_N = 1e7 / dwFrameDuration * 1000;
	m_videoFrame.frame_rate_D = 1001;
	m_videoFrame.picture_aspect_ratio = 16.0f / 9.0f;
	m_videoFrame.frame_format_type = NDIlib_frame_format_type_progressive;
	m_videoFrame.timecode = NDIlib_send_timecode_synthesize;
	m_videoFrame.line_stride_in_bytes = cbStride;

	if (dwFourcc == MWFOURCC_BGRA) {
		m_videoFrame.FourCC = NDIlib_FourCC_type_BGRA;
	} else if (dwFourcc == MWFOURCC_UYVY) {
		m_videoFrame.FourCC = NDIlib_FourCC_type_UYVY;
	} else
		return FALSE;
	m_videoFrame.FourCC = m_videoFrame.FourCC;

	//audio
	TCHAR szDevicePath[MAX_PATH];
	MWGetDevicePath(m_nIndex, szDevicePath);

	HCHANNEL t_hChannel = MWOpenChannelByPath(szDevicePath);
	if (t_hChannel == NULL) {
		return FALSE;
	}

	MWCAP_AUDIO_SIGNAL_STATUS audioStatus;
	MWGetAudioSignalStatus(t_hChannel, &audioStatus);
	int nAudioCount = 0;

	for (int i = 0; i < 4; i++) {
		if (audioStatus.wChannelValid & (0x01 << i)) {
			nAudioCount += 2;
		}
	}

	m_audioFrame.sample_rate = audioStatus.dwSampleRate;
	//m_audioFrame.no_channels = nAudioCount;
	m_audioFrame.no_channels = 2;
	m_audioFrame.no_samples = 192;
	m_audioFrame.timecode = 0LL;
	m_audioFrame.reference_level = 0;

	MWCloseChannel(t_hChannel);

	return TRUE;
}

LRESULT CVideoSend::OnMsgVideoSignalChanged(WPARAM w, LPARAM l)
{
	m_lockRecrete.Lock();
	CMWAutoLock lock(m_lock);
	SetNDICaptureFrameParams(m_cx, m_cy, m_dwFrameDuration, m_dwFourcc);
	m_lockRecrete.Unlock();

	return 0;
}

BOOL CVideoSend::GetNDIVar(NDIlib_send_instance_t *pNDI_send,
			   NDIlib_video_frame_v2_t *pVideoFrame,
			   NDIlib_audio_frame_interleaved_16s_t *pAudioFrame)
{
	*pNDI_send = m_pNDI_send;
	*pVideoFrame = m_videoFrame;
	*pAudioFrame = m_audioFrame;

	return TRUE;
}