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
#include "XICaptureQuad.h"
#include "ChildView.h"

#include "SettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CChildView

CChildView::CChildView()
{
	m_pVideoRenderer = NULL;
	m_hPowerNotify = NULL;

	m_dwFourcc = MWFOURCC_YUY2;

	m_nLayoutX = 2;
	m_nLayoutY = 2;

	m_cx = 1920;
	m_cy = 1080;

	m_dwFrameDuration = 333333;
	m_bSyncMode = TRUE;

	m_nNumChannel = MAX_CHANNEL_COUNT;
	for (int i = 0; i < MAX_CHANNEL_COUNT; ++i) {
		m_arrChannel[i].nBoard = 0;
		m_arrChannel[i].nChannel = i;
	}
}

CChildView::~CChildView() {}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()
//ON_WM_POWERBROADCAST()
ON_COMMAND(ID_FILE_SETTINGS, &CChildView::OnFileSettings)
ON_WM_ERASEBKGND()
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

	// TODO: Add your message handler code here

	// Do not call CWnd::OnPaint() for painting messages
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (IDOK != CallFileSettings()) {
		OnClose();
		return -1;
	}

	OpenPreview();

	//GUID gNotify = GUID_MONITOR_POWER_ON;
	//m_hPowerNotify = RegisterPowerSettingNotification(GetSafeHwnd(), &gNotify, DEVICE_NOTIFY_WINDOW_HANDLE);

	return 0;
}

void CChildView::OnDestroy()
{
	ClosePreview();

	if (m_hPowerNotify != NULL) {
		UnregisterPowerSettingNotification(m_hPowerNotify);
		m_hPowerNotify = NULL;
	}

	CWnd::OnDestroy();
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CChildView::OnVideoSignalChanged(int cx, int cy, DWORD dwParam) {}

void CChildView::OnCaptureCallback(const BYTE *pbyData, int cbStride,
				   DWORD dwParam)
{
	CMWAutoLock lock(m_lock);

	if (m_pVideoRenderer != NULL) {
		m_pVideoRenderer->paint((unsigned char *)pbyData);
	}
}

BOOL CChildView::OpenPreview()
{
	int nWidth = m_nLayoutX * m_cx;
	int nHeight = m_nLayoutY * m_cy;

	m_pVideoRenderer = new MWDXRender();
	bool t_b_reverse = false;
	if (!m_pVideoRenderer->initialize(
		    nWidth, nHeight, m_dwFourcc, t_b_reverse, GetSafeHwnd(),
		    MW_CSP_BT_709, MW_CSP_LEVELS_TV, MW_CSP_LEVELS_PC)) {
		return FALSE;
	}

	if (!m_thread.Create(m_arrChannel, m_nNumChannel, m_nLayoutX,
			     m_nLayoutY, m_cx, m_cy, m_dwFourcc,
			     m_dwFrameDuration, m_bSyncMode, this)) {
		AfxMessageBox(_T("Open device error!"));
		return FALSE;
	}

	return TRUE;
}

void CChildView::ClosePreview()
{
	m_thread.Destroy();

	if (m_pVideoRenderer) {
		m_pVideoRenderer->cleanup_device();
		delete m_pVideoRenderer;
		m_pVideoRenderer = NULL;
	}
}

UINT CChildView::OnPowerBroadcast(UINT nPowerEvent, UINT nEventData)
{
	if (nPowerEvent == PBT_POWERSETTINGCHANGE) {
		PPOWERBROADCAST_SETTING pSetting =
			(PPOWERBROADCAST_SETTING)nEventData;
		DWORD *pdwData = (DWORD *)pSetting->Data;
		if (*pdwData == 0x01) {
			OpenPreview();
		} else {
			ClosePreview();
		}
	}

	return __super::OnPowerBroadcast(nPowerEvent, nEventData);
}

INT_PTR CChildView::CallFileSettings()
{
	CSettingDlg dlg;
	dlg.m_cx = m_cx;
	dlg.m_cy = m_cy;
	dlg.m_dwFrameDuration = m_dwFrameDuration;
	dlg.m_nLayoutX = m_nLayoutX;
	dlg.m_nLayoutY = m_nLayoutY;

	dlg.m_nNumChannel = m_nNumChannel;
	for (int i = 0; i < m_nNumChannel; i++) {
		dlg.m_arrChannel[i] = m_arrChannel[i];
	}
	dlg.m_bSync = m_bSyncMode;

	INT_PTR uRet = dlg.DoModal();
	if (IDOK == uRet) {
		m_cx = dlg.m_cx;
		m_cy = dlg.m_cy;
		m_dwFrameDuration = dlg.m_dwFrameDuration;
		m_nLayoutX = dlg.m_nLayoutX;
		m_nLayoutY = dlg.m_nLayoutY;
		m_nNumChannel = dlg.m_nNumChannel;

		for (int i = 0; i < m_nNumChannel; i++) {
			m_arrChannel[i] = dlg.m_arrChannel[i];
		}
		m_bSyncMode = dlg.m_bSync;
	}
	return uRet;
}

void CChildView::OnFileSettings()
{
	if (IDOK == CallFileSettings()) {
		ClosePreview();
		OpenPreview();
	}
}

BOOL CChildView::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

BOOL CChildView::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN))

		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}
