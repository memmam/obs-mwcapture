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
#include "OSDPreview.h"

#include "StringUtils.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
ON_WM_CREATE()
ON_WM_SETFOCUS()
ON_COMMAND_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16, OnChannelItem)
ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16,
			   OnUpdateChannelItem)
ON_WM_MOVE()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_nChlCount = 0;
	m_nCurChl = -1;
}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			      CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST,
			      NULL)) {
		TRACE0("Failed to create view window\n");
		return -1;
	}

	// Init Video Device
	CMenu *pMenu = GetMenu();
	CMenu *pSubMenu = pMenu->GetSubMenu(1);

	m_nChlCount = g_nValidChannelCount;
	TCHAR szChannelName[MAX_PATH];
	for (int i = 0; i < m_nChlCount; i++) {
		MWCAP_CHANNEL_INFO chlInfo;
		if (MW_SUCCEEDED ==
		    MWGetChannelInfoByIndex(g_nValidChannel[i], &chlInfo)) {
			CAutoConvertString strName(chlInfo.szProductName);
			_stprintf_s(szChannelName, _T("%s %d-%d\0"),
				    (const TCHAR *)strName,
				    chlInfo.byBoardIndex,
				    chlInfo.byChannelIndex);
			pSubMenu->InsertMenuW(i + 1, MF_BYPOSITION,
					      ID_DEVICE_BEGIN + 1 + i,
					      szChannelName);
		}
	}

	pSubMenu->DeleteMenu(0, MF_BYPOSITION);

	if (m_nChlCount > 0) {
		OnChannelItem(ID_DEVICE_BEGIN + 1);
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT &cs)
{
	if (!CFrameWnd::PreCreateWindow(cs))
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

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

void CMainFrame::Dump(CDumpContext &dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG

// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd * /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void *pExtra,
			  AFX_CMDHANDLERINFO *pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnChannelItem(UINT nID)
{
	if (nID <= (UINT)(ID_DEVICE_BEGIN + m_nChlCount)) {
		int nCurChl = nID - ID_DEVICE_BEGIN - 1;
		if (m_nCurChl == nCurChl)
			return;

		if (m_wndView.SelectChannel(nCurChl)) {
			m_nCurChl = nCurChl;
		} else {
			AfxMessageBox(_T("Select Channel error!"));
		}
	}
}

void CMainFrame::OnUpdateChannelItem(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID <= (UINT)(ID_DEVICE_BEGIN + m_nChlCount)) {
		if (pCmdUI->m_nID == ID_DEVICE_BEGIN + 1 + m_nCurChl)
			pCmdUI->SetCheck(TRUE);
		else
			pCmdUI->SetCheck(FALSE);
	}

	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
}
