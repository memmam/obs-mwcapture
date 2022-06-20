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
#include "MultiStreaming.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView

IMPLEMENT_DYNCREATE(CChildView, CView);

CChildView::CChildView()
{
	m_pRenderer = NULL;
	m_hChannel = NULL;

	m_hExitEvent = NULL;
	m_hCaptureThread = NULL;

	m_dCurrentFps = 0.0;

	m_nWidth = 0;
	m_nHeight = 0;
}

CChildView::~CChildView() {}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()

END_MESSAGE_MAP()

// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT &cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW),
		reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Do not call CWnd::OnPaint() for painting messages
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	StopPreview();
}

BOOL CChildView::StartPreview(int nChannelId, int cx, int cy)
{
	do {
		m_nWidth = cx;
		m_nHeight = cy;

		// Init capture
		TCHAR szDevicePath[MAX_PATH];
		if (MW_SUCCEEDED != MWGetDevicePath(g_nValidChannel[nChannelId],
						    szDevicePath)) {
			MessageBox(_T("MWGetDevicePath fail !\n"));
			break;
		}

		m_hChannel = MWOpenChannelByPath(szDevicePath);
		if (m_hChannel == NULL) {
			MessageBox(_T("Open video capture fail !\n"));
			break;
		}

		MWCAP_VIDEO_SIGNAL_STATUS t_status;
		MWGetVideoSignalStatus(m_hChannel, &t_status);
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

		// Init renderer
		m_pRenderer = new MWDXRender();
		bool t_b_reverse = false;
		if (CAPTURE_COLORSPACE == MWFOURCC_BGR24 ||
		    CAPTURE_COLORSPACE == MWFOURCC_BGRA)
			t_b_reverse = true;
		if (!m_pRenderer->initialize(m_nWidth, m_nHeight,
					     CAPTURE_COLORSPACE, t_b_reverse,
					     GetSafeHwnd(), color_format,
					     input_range, output_range)) {
			MessageBox(_T("Init renderer fail !\n"));
			break;
		}

		m_dCurrentFps = 0.0;

		m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (m_hExitEvent == NULL)
			break;

		m_hCaptureThread = CreateThread(NULL, 0, VideoThreadProc,
						(LPVOID)this, 0, NULL);
		if (m_hCaptureThread == NULL)
			break;

		return TRUE;
	} while (FALSE);

	StopPreview();
	return FALSE;
}

void CChildView::StopPreview()
{
	if (m_hCaptureThread != NULL) {
		if (m_hExitEvent != NULL)
			SetEvent(m_hExitEvent);

		WaitForSingleObject(m_hCaptureThread, INFINITE);

		CloseHandle(m_hCaptureThread);
		m_hCaptureThread = NULL;
	}

	if (m_hExitEvent != NULL) {
		CloseHandle(m_hExitEvent);
		m_hExitEvent = NULL;
	}

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	if (m_pRenderer != NULL) {
		m_pRenderer->cleanup_device();
		delete m_pRenderer;
		m_pRenderer = NULL;
	}
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

DWORD CChildView::VideoThreadProcEx()
{
	// Preview
	DWORD cbStride = FOURCC_CalcMinStride(CAPTURE_COLORSPACE, m_nWidth, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(CAPTURE_COLORSPACE, m_nWidth,
						 m_nHeight, cbStride);

	BYTE *byBuffer = NULL;
	byBuffer = new BYTE[dwFrameSize];
	memset(byBuffer, 0xFF, dwFrameSize);

	// Wait Events
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hTimerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	MW_RESULT xr;
	do {
		xr = MWStartVideoCapture(m_hChannel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;

		xr = MWPinVideoBuffer(m_hChannel, byBuffer, dwFrameSize);
		if (xr != MW_SUCCEEDED)
			OutputDebugStringA("MWPinVideoBuffer fail !\n");

		HTIMER hTimerNotify = MWRegisterTimer(m_hChannel, hTimerEvent);
		if (hTimerNotify == NULL)
			break;

		DWORD dwFrameCount = 0;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		LONGLONG llExpireTime = llBegin;
		DWORD dwFrameDuration = FRAME_DURATION;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llLast = llBegin;

		HANDLE aEventNotify[] = {m_hExitEvent, hTimerEvent};
		const int nEventNotify =
			sizeof(aEventNotify) / sizeof(aEventNotify[0]);

		HANDLE aEventCapture[] = {m_hExitEvent, hCaptureEvent};
		const int nEventCapture =
			sizeof(aEventCapture) / sizeof(aEventCapture[0]);

		while (TRUE) {
			llExpireTime = llExpireTime + dwFrameDuration;
			xr = MWScheduleTimer(m_hChannel, hTimerNotify,
					     llExpireTime);
			if (xr != MW_SUCCEEDED) {
				continue;
			}

			DWORD dwRet = WaitForMultipleObjects(
				nEventNotify, aEventNotify, FALSE, INFINITE);
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

				xr = MWCaptureVideoFrameToVirtualAddress(
					m_hChannel,
					MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED,
					byBuffer, dwFrameSize, cbStride, FALSE,
					NULL, CAPTURE_COLORSPACE, m_nWidth,
					m_nHeight);

				dwRet = WaitForMultipleObjects(nEventCapture,
							       aEventCapture,
							       FALSE, INFINITE);
				if (dwRet == WAIT_OBJECT_0) {
					break;
				}

				xr = MWGetVideoCaptureStatus(m_hChannel,
							     &captureStatus);

				if (m_pRenderer != NULL) {
					m_pRenderer->paint(byBuffer);
				}

				dwFrameCount++;

				LONGLONG llCurrent = 0LL;
				MWGetDeviceTime(m_hChannel, &llCurrent);

				if (llCurrent - llLast >= 10000000LL) {
					m_dCurrentFps = (double)dwFrameCount *
							10000000LL /
							(llCurrent - llLast);
				}
			}
		}

		xr = MWUnregisterTimer(m_hChannel, hTimerNotify);
		xr = MWStopVideoCapture(m_hChannel);

		MWUnpinVideoBuffer(m_hChannel, byBuffer);
	} while (FALSE);

	if (hCaptureEvent != NULL)
		CloseHandle(hCaptureEvent);

	if (hTimerEvent != NULL)
		CloseHandle(hTimerEvent);

	if (byBuffer != NULL) {
		delete[] byBuffer;
	}

	return 0;
}

void CChildView::OnDraw(CDC *pDC) {}

BOOL CChildView::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN))

		return TRUE;

	return CView::PreTranslateMessage(pMsg);
}
