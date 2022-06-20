/************************************************************************************************/
// ChildView.cpp : implementation of the CChildView class

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
#include "AVCapture2.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView

CChildView::CChildView()
{
	m_hChannel = NULL;
	m_hVideo = NULL;
	m_hAudio = NULL;
	m_pDX11VideoRenderer=NULL;

	m_pAudioRenderer = NULL;

	m_pBuf = (BYTE*)malloc(PREVIEW_WIDTH * PREVIEW_HEIGHT * 4);
	memset(m_pBuf, 0, PREVIEW_WIDTH * PREVIEW_HEIGHT * 4);

	m_n_height=PREVIEW_HEIGHT;
	m_n_width=PREVIEW_WIDTH;
	m_d_fps=0.0;
}

CChildView::~CChildView()
{
	if(m_pBuf != NULL)
	{
		free(m_pBuf);
		m_pBuf = NULL;
	}
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()

	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CChildView::OpenPreview(int nIndex, AudioType audioType, bool audioRender)
{
	MW_RESULT mr;

	WCHAR path[256] = {0};
	mr = MWGetDevicePath(nIndex, path);
	m_hChannel = MWOpenChannelByPath(path);
	if (m_hChannel == NULL)
	{
		MessageBox(L"Open channel failed!\n", L"AVCapture2");
		return FALSE;
	}

	MWCAP_VIDEO_SIGNAL_STATUS t_status;
	MWGetVideoSignalStatus(m_hChannel,&t_status);
	int t_n_width=PREVIEW_WIDTH;
	int t_n_height=PREVIEW_HEIGHT;
	int t_n_duration=PREVIEW_DURATION;
	MWCAP_VIDEO_COLOR_FORMAT			t_colorFormat = MWCAP_VIDEO_COLOR_FORMAT_YUV709;
	MWCAP_VIDEO_QUANTIZATION_RANGE		t_quantRange = MWCAP_VIDEO_QUANTIZATION_LIMITED;
	if(t_status.state==MWCAP_VIDEO_SIGNAL_LOCKED){
		t_n_width=t_status.cx;
		t_n_height=t_status.cy;
		t_n_duration=t_status.dwFrameDuration;
		t_colorFormat = t_status.colorFormat;
		t_quantRange = t_status.quantRange;
	}
	else{
		MWCAP_VIDEO_CAPS t_caps;
		MWGetVideoCaps(m_hChannel,&t_caps);
		t_n_width=t_caps.wMaxOutputWidth>=PREVIEW_WIDTH?PREVIEW_WIDTH:t_caps.wMaxOutputWidth;
		t_n_height=t_caps.wMaxOutputHeight>=PREVIEW_HEIGHT?PREVIEW_HEIGHT:t_caps.wMaxOutputHeight;
	}
	m_n_width=t_n_width;
	m_n_height=t_n_height;
	m_d_fps=10000000.0/t_n_duration;

	MWCAP_VIDEO_PROCESS_SETTINGS t_setting;
	MWGetVideoCaptureProcessSettings(m_hChannel,&t_setting);
	t_setting.rectSource.left = 0;
	t_setting.rectSource.top = 0;
	t_setting.rectSource.right = 0;
	t_setting.rectSource.bottom =0;
	MWSetVideoCaptureProcessSettings(m_hChannel,&t_setting);

	m_hVideo = MWCreateVideoCapture(m_hChannel,t_n_width, t_n_height, PREVIEW_FOURCC, t_n_duration, OnVideoCaptureCallback, this);
	if (m_hVideo == NULL)
	{
		MessageBox(L"Create video capture failed !", L"AVCapture2");
		return FALSE;
	}
	CMWAutoLock lock(m_lockVideo);
	m_pDX11VideoRenderer = new MWDXRender();
	bool t_b_reverse=false;
	if(PREVIEW_FOURCC==MWFOURCC_BGR24||PREVIEW_FOURCC==MWFOURCC_BGRA) t_b_reverse=true;
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

	if (!m_pDX11VideoRenderer->initialize(t_n_width, t_n_height, PREVIEW_FOURCC,t_b_reverse,GetSafeHwnd(), color_format, input_range, output_range)) {
		MessageBox(L"Create video render failed !", L"AVCapture2");
		delete m_pDX11VideoRenderer;
		m_pDX11VideoRenderer = NULL;
		return FALSE;
	}

	if (audioRender == true)
	{
		MWCAP_AUDIO_CAPTURE_NODE t_node = MWCAP_AUDIO_CAPTURE_NODE_DEFAULT;
		switch (audioType)
		{
		case DEFAULT:
			break;
		case EMBEDDED:
			t_node = MWCAP_AUDIO_CAPTURE_NODE_EMBEDDED_CAPTURE;
			break;
		case MICROPHONE:
			t_node = MWCAP_AUDIO_CAPTURE_NODE_MICROPHONE;
			break;
		case USB_CAPTURE:
			t_node = MWCAP_AUDIO_CAPTURE_NODE_USB_CAPTURE;
			break;
		case LINE_IN:
			t_node = MWCAP_AUDIO_CAPTURE_NODE_LINE_IN;
			break;
		default:
			break;
		}

		m_hAudio = MWCreateAudioCapture(m_hChannel, t_node, 48000, 16, 2, OnAudioCaptureCallback, this);

		if (m_hAudio == NULL)
			MessageBox(L"Create audio capture failed !", L"AVCapture2");
		else
		{
			CMWAutoLock lock1(m_lockAudio);
			BOOL bRet = FALSE;
			do {
				m_pAudioRenderer = new CDSoundRenderer();
				if (!m_pAudioRenderer)
				{
					MessageBox(L"Create audio render failed !", L"AVCapture2");
					break;
				}

				GUID guid = GUID_NULL;
				if (!m_pAudioRenderer->Initialize(&guid))
				{
					MessageBox(L"Create audio render failed !", L"AVCapture2");
					break;
				}

				if (!m_pAudioRenderer->Create(48000, 2, 480, DEF_AUDIO_JITTER_FRAMES))
				{
					MessageBox(L"Create audio render failed !", L"AVCapture2");
					delete m_pAudioRenderer;
					m_pAudioRenderer = NULL;
					break;
				}

				if (!m_pAudioRenderer->Run())
				{
					MessageBox(L"Create audio render failed !", L"AVCapture2");
					delete m_pAudioRenderer;
					m_pAudioRenderer = NULL;
					break;
				}


				bRet = TRUE;
			} while (FALSE);
		}
	}
	return TRUE;
}

void CChildView::ClosePreview()
{
	if (m_hVideo != NULL)
	{
		MWDestoryVideoCapture(m_hVideo);
		m_hVideo = NULL;
	}

	if (m_hAudio != NULL)
	{
		MWDestoryAudioCapture(m_hAudio);
		m_hAudio = NULL;
	}

	if (m_hChannel != NULL)
	{
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}
	if(m_pDX11VideoRenderer!=NULL){
		m_pDX11VideoRenderer->cleanup_device();
		delete m_pDX11VideoRenderer;
		m_pDX11VideoRenderer=NULL;
	}

	if (m_pAudioRenderer != NULL)
	{
		delete m_pAudioRenderer;
		m_pAudioRenderer = NULL;
	}
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}


BOOL CChildView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam
		&& (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE 
		|| pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT
		|| pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP
		|| pMsg->wParam == VK_DOWN))

		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}

HCHANNEL CChildView::GetChannel()
{
	return m_hChannel;
}