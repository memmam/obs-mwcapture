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
#include "AVCapture.h"

#include "MainFrm.h"
#include "StringUtils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOVE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
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
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CMenu* pMenu = GetMenu();
    CMenu* pSubMenu = pMenu->GetSubMenu(1);
	for (int i = 0; i < g_nValidChannelCount; i ++) {
		MWCAP_CHANNEL_INFO mci;
		if (MW_SUCCEEDED == MWGetChannelInfoByIndex(g_nValidChannel[i], &mci)) {
				CString strName;
				CAutoConvertString strProductName(mci.szProductName);
				strName.Format(_T("%02d-%d %s"), mci.byBoardIndex, mci.byChannelIndex, (const TCHAR*)strProductName);

				pSubMenu->InsertMenu(i + 1, MF_BYPOSITION, ID_DEVICE_BEGIN + 1 + i, strName);
		}
	}

	pSubMenu->DeleteMenu(0, MF_BYPOSITION);

	CMenu* pMenuFile = pMenu->GetSubMenu(0);
	CMenu* pMenuRecord = pMenuFile->GetSubMenu(0);
	vector<mw_venc_gpu_info_t> t_vec_info = m_wndView.GetGPUInfos();
	for(int i=0;i<t_vec_info.size();i++){
		WCHAR t_wcs_name[128]={0};
		swprintf_s(t_wcs_name,L"%S",t_vec_info.at(i).gpu_name);
		pMenuRecord->InsertMenu(1,MF_BYPOSITION,ID_STARTRECORD_CPU+1,t_wcs_name);
	}

	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
	
	SetTimer(1, 500, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
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

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnDestroy()
{
	KillTimer(1);

	CFrameWnd::OnDestroy();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		int cx, cy;
		MWCAP_VIDEO_COLOR_FORMAT colorFormat;
		MWCAP_VIDEO_QUANTIZATION_RANGE quantRange;
		m_wndView.GetVideoFormat(cx, cy, colorFormat, quantRange);

		CString strInfo;
		strInfo.Format(_T("%d x %d  NV12  %.02lf FPS"), cx, cy, m_wndView.GetPreviewFPS());
		m_wndStatusBar.SetWindowText (strInfo);
	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CMainFrame::OnMove(int x, int y)
{
	CWnd::OnMove(x, y);

}