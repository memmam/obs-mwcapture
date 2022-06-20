/************************************************************************************************/
// MainFrm.cpp : implementation of the CMainFrame class

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

#include "MainFrm.h"

#include "StringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_ID				1

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_MOVE()
	ON_WM_TIMER()

	ON_COMMAND_RANGE(ID_CHANNEL_, ID_CHANNEL_ + 16, OnChannelItem)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CHANNEL_, ID_CHANNEL_ + 16, OnUpdateChannelItem)
	//ON_WM_POWERBROADCAST()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_nSelChannelIndex = -1;
	m_nChannelIndex = -1;
	m_nChannelCount = 0;

	m_hPowerNotify = NULL;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Init Video Device
	CMenu* pMenu = GetMenu();
    CMenu* pSubMenu = pMenu->GetSubMenu(1);

	m_nChannelCount = 0;
	m_nSelChannelIndex = 0;

	TCHAR szChannelName[MAX_PATH];
	MWCAP_CHANNEL_INFO info;
	for (int i = 0; i < g_nValidChannelCount; i ++) {
		if (MW_SUCCEEDED != MWGetChannelInfoByIndex(g_nValidChannel[i], &info))
			continue;
		
		_stprintf_s(szChannelName, _T("%s %d-%d\0"), (const TCHAR*)CAutoConvertString(info.szProductName), (int)info.byBoardIndex, (int)info.byChannelIndex);
		pSubMenu->InsertMenu(m_nChannelCount + 1, MF_BYPOSITION, ID_CHANNEL_ + 1 + m_nChannelCount, szChannelName);

		m_nChannelCount++;

		if (m_nChannelIndex < 0)
			m_nChannelIndex = i;
	}
		
	pSubMenu->DeleteMenu(0, MF_BYPOSITION);

	if (m_nChannelCount > 0) {
		OnChannelItem(ID_CHANNEL_ + 1 + 0);
	}

	return 0;
}

void CMainFrame::OnDestroy()
{
	KillTimer(1);

	if (m_hPowerNotify != NULL) {
		UnregisterPowerSettingNotification(m_hPowerNotify);
		m_hPowerNotify = NULL;
	}
	
	m_wndSplitterLeft.DestroyWindow();
	m_wndSplitterRight.DestroyWindow();
	m_wndSplitterMain.DestroyWindow();
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.cx = 1700;
	cs.cy = 700;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
}

void CMainFrame::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	if (!m_wndSplitterMain.CreateStatic(this, 1, 2))
		return FALSE;
	
	if (!m_wndSplitterLeft.CreateStatic(&m_wndSplitterMain, 2, 1, WS_CHILD|WS_VISIBLE, m_wndSplitterMain.IdFromRowCol(0, 0))) {
		m_wndSplitterMain.DestroyWindow();
		return FALSE;
	}

	if (!m_wndSplitterRight.CreateStatic(&m_wndSplitterMain, 4, 1, WS_CHILD|WS_VISIBLE, m_wndSplitterMain.IdFromRowCol(0, 1))) {
		m_wndSplitterLeft.DestroyWindow();
		m_wndSplitterMain.DestroyWindow();
		return FALSE;
	}

	do {
		CRect rect;
		GetClientRect(&rect);
		// Status
		
		if (!m_wndSplitterLeft.CreateView(0, 0, RUNTIME_CLASS(CStatusView), CSize(rect.Width() * 3 / 5, STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}

		if (!m_wndSplitterRight.CreateView(0, 0, RUNTIME_CLASS(CStatusView), CSize(rect.Width() * 2 / 5, STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}
		
		if (!m_wndSplitterRight.CreateView(2, 0, RUNTIME_CLASS(CStatusView), CSize(rect.Width() * 2 / 5, STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}

		// Video
		if (!m_wndSplitterLeft.CreateView(1, 0, RUNTIME_CLASS(CChildView), CSize(rect.Width() * 3 / 5, rect.Height() - STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}
		
		if (!m_wndSplitterRight.CreateView(1, 0, RUNTIME_CLASS(CChildView), CSize(rect.Width() * 2 / 5, rect.Height() / 2 - STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}
		
		if (!m_wndSplitterRight.CreateView(3, 0, RUNTIME_CLASS(CChildView), CSize(rect.Width() * 2 / 5, rect.Height() / 2 - STATUS_AREA_HEIGHT), pContext)) {
			return FALSE;
		}
		
		m_wndSplitterMain.SetColumnInfo(0, rect.Width() * 3 / 5, 0);

		SetTimer(1, 1000, NULL);
		
		GUID gNotify = GUID_MONITOR_POWER_ON;
		m_hPowerNotify = RegisterPowerSettingNotification(GetSafeHwnd(), &gNotify, DEVICE_NOTIFY_WINDOW_HANDLE);

		return TRUE;
	} while (FALSE);
	
	m_wndSplitterLeft.DestroyWindow();
	m_wndSplitterRight.DestroyWindow();
	m_wndSplitterMain.DestroyWindow();

	return FALSE;
}

BOOL CMainFrame::StartPreview()
{
	CChildView* szChildView[CHANNEL_VIEW_COUNT];
	szChildView[0] = (CChildView*)m_wndSplitterLeft.GetPane(1, 0);
	szChildView[1] = (CChildView*)m_wndSplitterRight.GetPane(1, 0);
	szChildView[2] = (CChildView*)m_wndSplitterRight.GetPane(3, 0);

	for (int i = 0; i < CHANNEL_VIEW_COUNT; ++i) {
		if (szChildView[i] == NULL) 
			return FALSE;
	}

	for (int i = 0; i < CHANNEL_VIEW_COUNT; ++i) {
		szChildView[i]->StopPreview();
	}

	for (int i = 0; i < CHANNEL_VIEW_COUNT; ++i) {
		if (!szChildView[i]->StartPreview(m_nChannelIndex, g_szResolution[i][0], g_szResolution[i][1]))
			AfxMessageBox(_T("Open device error!"));
	}

	return TRUE;
}

void CMainFrame::StopPreview()
{
	CChildView* szChildView[CHANNEL_VIEW_COUNT];
	szChildView[0] = (CChildView*)m_wndSplitterLeft.GetPane(1, 0);
	szChildView[1] = (CChildView*)m_wndSplitterRight.GetPane(1, 0);
	szChildView[2] = (CChildView*)m_wndSplitterRight.GetPane(3, 0);

	for (int i = 0; i < CHANNEL_VIEW_COUNT; ++i) {
		if (szChildView[i])
			szChildView[i]->StopPreview();
	}
}

void CMainFrame::OnChannelItem(UINT nID)
{
	if (nID <= (UINT)(ID_CHANNEL_ + m_nChannelCount)) {	
		int nSelChannelIndex = nID - ID_CHANNEL_ - 1;

		int nChannelIndex = 0;
		MWCAP_CHANNEL_INFO info;
		for (; nChannelIndex < g_nValidChannelCount && (nSelChannelIndex > 0); ++nChannelIndex) {
			if (MW_SUCCEEDED != MWGetChannelInfoByIndex(g_nValidChannel[nChannelIndex], &info))
				continue;
			
			nSelChannelIndex--;
		}

		m_nChannelIndex = nChannelIndex;
		m_nSelChannelIndex = nID - ID_CHANNEL_ - 1;

		StartPreview();
	}
}

void CMainFrame::OnUpdateChannelItem(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID <= (UINT)(ID_CHANNEL_ + m_nChannelCount)) {
		if (pCmdUI->m_nID == ID_CHANNEL_ + 1 + m_nSelChannelIndex)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}

	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		CChildView* szChildView[CHANNEL_VIEW_COUNT];
		szChildView[0] = (CChildView*)m_wndSplitterLeft.GetPane(1, 0);
		szChildView[1] = (CChildView*)m_wndSplitterRight.GetPane(1, 0);
		szChildView[2] = (CChildView*)m_wndSplitterRight.GetPane(3, 0);

		CStatusView* szStatusView[CHANNEL_VIEW_COUNT];
		szStatusView[0] = (CStatusView*)m_wndSplitterLeft.GetPane(0, 0);
		szStatusView[1] = (CStatusView*)m_wndSplitterRight.GetPane(0, 0);
		szStatusView[2] = (CStatusView*)m_wndSplitterRight.GetPane(2, 0);

		int cx = 0;
		int cy = 0;
		DWORD dwFmt = MWFOURCC_UNK;
		double dFps = 0.0;
		for (int i = 0; i < CHANNEL_VIEW_COUNT; ++i) {
			if (szStatusView[i]) {
				if (szChildView[i])
					szChildView[i]->GetCaptureInfo(&cx, &cy, &dwFmt, &dFps);
				szStatusView[i]->UpdateStatus(cx, cy, dwFmt, dFps);
			}
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}

UINT CMainFrame::OnPowerBroadcast(UINT nPowerEvent, UINT nEventData)
{
	if (nPowerEvent == PBT_POWERSETTINGCHANGE) {
		PPOWERBROADCAST_SETTING pSetting = (PPOWERBROADCAST_SETTING)nEventData;
		DWORD * pdwData = (DWORD*)pSetting->Data;
		if (*pdwData != 0x00) {
			StartPreview();
		}
		else {
			StopPreview();
		}
	}

	return CFrameWnd::OnPowerBroadcast(nPowerEvent, nEventData);
}
