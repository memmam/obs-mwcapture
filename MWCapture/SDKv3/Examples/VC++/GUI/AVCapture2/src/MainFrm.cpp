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
#include "AVCapture2.h"

#include "MainFrm.h"
#include "StringUtils.h"

#include <afxconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_EXIT, &CMainFrame::OnAppExit)
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_COMMAND_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16, OnDeviceItem)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16, OnUpdateDeviceItem)
	ON_COMMAND_RANGE(ID_AUDIO_BEGIN, ID_AUDIO_BEGIN + 5, OnAudioItem)
	ON_UPDATE_COMMAND_UI_RANGE(ID_AUDIO_BEGIN, ID_AUDIO_BEGIN + 5, OnUpdateAudioItem)

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
	m_nIndex = 0;
	m_audioType = DEFAULT;
	m_bSelectAudio = NULL;
}

CMainFrame::~CMainFrame()
{
	if (m_bSelectAudio != NULL)
	{
		delete[] m_bSelectAudio;
		m_bSelectAudio = NULL;
	}
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nNumChannel = MWGetChannelCount();
	CString defaultCStrName;
	CString strName;

	CMenu* pMenu = GetMenu();
	CMenu* pSubMenu = pMenu->GetSubMenu(1);
	for (int i = 0; i < nNumChannel; i ++) {
		MWCAP_CHANNEL_INFO mci;
		if (MW_SUCCEEDED == MWGetChannelInfoByIndex(i, &mci)) {
			if (strcmp(mci.szFamilyName, "USB Capture") == 0)
			{
				WCHAR chName[128] = {0};
				MWGetDevicePath(i, chName);
				HCHANNEL hChannel = MWOpenChannelByPath(chName);
				if (hChannel == NULL)
					continue;

				MWCAP_DEVICE_NAME_MODE mode;
				MW_RESULT mr = MWUSBGetDeviceNameMode(hChannel, &mode);
				if (mode == MWCAP_DEVICE_NAME_SERIAL_NUMBER)
				{
					CAutoConvertString strProductName(mci.szBoardSerialNo);
					strName.Format(_T("%s"), (const TCHAR*)strProductName);
					pSubMenu->InsertMenu(i + 1, MF_BYPOSITION, ID_DEVICE_BEGIN + 1 + i, strName);
				}
				else
				{
					CAutoConvertString strProductName(mci.szBoardSerialNo);
					CAutoConvertString strPname(mci.szProductName);
					strName.Format(_T("%s(%s)"),(const TCHAR*)strPname,(const TCHAR*)strProductName);
					pSubMenu->InsertMenu(i + 1, MF_BYPOSITION, ID_DEVICE_BEGIN + 1 + i, strName);
				}

				if (hChannel != NULL)
				{
					MWCloseChannel(hChannel);
					hChannel = NULL;
				}
			}
			else
			{
				CAutoConvertString strProductName(mci.szProductName);
				strName.Format(_T("%02d-%d %s"), mci.byBoardIndex, mci.byChannelIndex, (const TCHAR*)strProductName);
				pSubMenu->InsertMenu(i + 1, MF_BYPOSITION, ID_DEVICE_BEGIN + 1 + i, strName);
			}
		}

		if (i == m_nIndex)
			defaultCStrName = strName;
	}

	pSubMenu = pMenu->GetSubMenu(2);

	USES_CONVERSION;
	if (strstr(W2A(defaultCStrName), "Pro Capture") != NULL||
		strstr(W2A(defaultCStrName), "Eco Capture") != NULL)
	{
		pSubMenu->InsertMenu(1, MF_BYPOSITION, ID_AUDIO_BEGIN + 1, defaultCStrName);
	}
	else if (strstr(W2A(defaultCStrName), "USB Capture") != NULL)
	{
		WCHAR path[128];
		MWGetDevicePath(m_nIndex, path);
		HCHANNEL hChannel = MWOpenChannelByPath(path);

		MWCAP_AUDIO_CAPS t_Caps;
		MWGetAudioCaps(hChannel, &t_Caps);

		int audioTypeNum = 0;
		CString showName;
		if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_EMBEDDED_CAPTURE)
		{
			showName.Format(_T("Digital (%s)"), defaultCStrName);
			pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
			audioTypeNum++;
		}
		if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_MICROPHONE)
		{
			showName.Format(_T("Mic (%s)"), defaultCStrName);
			pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
			audioTypeNum++;
		}		
		if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_USB_CAPTURE)
		{
			showName.Format(_T("Computer (%s)"), defaultCStrName);
			pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
			audioTypeNum++;
		}
		if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_LINE_IN)
		{
			showName.Format(_T("Analog (%s)"), defaultCStrName);
			pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
			audioTypeNum++;
		}

		MWCloseChannel(hChannel);
		hChannel = NULL;
	}

	pSubMenu->DeleteMenu(0, MF_BYPOSITION);

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

	BOOL bRet = m_wndView.OpenPreview(m_nIndex, m_audioType);

	int nSelectDeviceAudioType = m_audioType;
	CString audioStr;
	CString str;

	if (nSelectDeviceAudioType == -1)
	{
		HCHANNEL hChannel = m_wndView.GetChannel();
		DWORD dwAudioInput = 0;
		MW_RESULT xr = MWGetAudioInputSource(hChannel, &dwAudioInput);
		if (xr == MW_SUCCEEDED)
		{
			switch (INPUT_TYPE(dwAudioInput))
			{
			case MWCAP_AUDIO_INPUT_TYPE_NONE:
				break;
			case MWCAP_AUDIO_INPUT_TYPE_HDMI:
			case MWCAP_AUDIO_INPUT_TYPE_SDI:
				audioStr.Format(_T("Digital"));
				break;
			case MWCAP_AUDIO_INPUT_TYPE_LINE_IN:
				audioStr.Format(_T("Analog"));
				break;
			case MWCAP_AUDIO_INPUT_TYPE_MIC_IN:
				audioStr.Format(_T("Mic"));
				break;
			}
		}

		int nItemCount = pSubMenu->GetMenuItemCount();
		m_bSelectAudio = new bool[nItemCount];
		for (int i = 0; i < nItemCount; i++)
		{
			pSubMenu->GetMenuStringW(ID_AUDIO_BEGIN + 1 + i, str, NULL);
			if (strstr(W2A(str), "Pro Capture") != NULL||
				strstr(W2A(str), "Eco Capture") != NULL)
			{
				m_bSelectAudio[0] = true;
				break;
			}
			else if (strstr(W2A(str), W2A(audioStr)) != NULL)
			{
				m_bSelectAudio[i] = true;
				break;
			}
		}
	}

	SetTimer(1,500,NULL);

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

void CMainFrame::OnDeviceItem(UINT nID)
{
	m_wndView.ClosePreview();
	m_audioType = DEFAULT;

	int nChannel = nID - ID_DEVICE_BEGIN - 1;

	CMenu* pMenu = GetMenu();
	CMenu* pSubMenu = pMenu->GetSubMenu(2);
	int nItemCount = pSubMenu->GetMenuItemCount();
	for(int i = nItemCount - 1; i >= 0; i--)
		pSubMenu->DeleteMenu(0, MF_BYPOSITION);

	if (m_bSelectAudio != NULL)
	{
		delete[] m_bSelectAudio;
		m_bSelectAudio = NULL;
	}

	CString strName;
	MWCAP_CHANNEL_INFO mci;
	if (MW_SUCCEEDED == MWGetChannelInfoByIndex(nChannel, &mci))
	{
		if (strcmp(mci.szFamilyName, "Pro Capture") == 0||
			strcmp(mci.szFamilyName, "Eco Capture") == 0)
		{
			CAutoConvertString strProductName(mci.szProductName);
			strName.Format(_T("%02d-%d %s"), mci.byBoardIndex, mci.byChannelIndex, (const TCHAR*)strProductName);
			pSubMenu->InsertMenu(1, MF_BYPOSITION, ID_AUDIO_BEGIN + 1, strName);
		}
		else if (strcmp(mci.szFamilyName, "USB Capture") == 0)
		{
			WCHAR chName[128] = { 0 };
			MWGetDevicePath(nChannel, chName);
			HCHANNEL hChannel = MWOpenChannelByPath(chName);
			if (hChannel == NULL)
				return;

			MWCAP_DEVICE_NAME_MODE mode;
			MW_RESULT mr = MWUSBGetDeviceNameMode(hChannel, &mode);
			if (mode == MWCAP_DEVICE_NAME_SERIAL_NUMBER)
			{
				CAutoConvertString strProductName(mci.szBoardSerialNo);
				strName.Format(_T("%s"), (const TCHAR*)strProductName);
			}
			else
			{
				CAutoConvertString strProductName(mci.szBoardSerialNo);
				CAutoConvertString strPname(mci.szProductName);
				strName.Format(_T("%s(%s)"), (const TCHAR*)strPname, (const TCHAR*)strProductName);
			}

			MWCAP_AUDIO_CAPS t_Caps;
			MWGetAudioCaps(hChannel, &t_Caps);

			int audioTypeNum = 0;
			CString showName;
			if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_EMBEDDED_CAPTURE)
			{
				showName.Format(_T("Digital (%s)"), strName);
				pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
				audioTypeNum++;
			}
			if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_MICROPHONE)
			{
				showName.Format(_T("Mic (%s)"), strName);
				pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
				audioTypeNum++;
			}
			if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_USB_CAPTURE)
			{
				showName.Format(_T("Computer (%s)"), strName);
				pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
				audioTypeNum++;
			}
			if (t_Caps.dwCaps & MWCAP_USB_AUDIO_CAP_LINE_IN)
			{
				showName.Format(_T("Analog (%s)"), strName);
				pSubMenu->InsertMenu(audioTypeNum + 1, MF_BYPOSITION, ID_AUDIO_BEGIN + audioTypeNum + 1, showName);
				audioTypeNum++;
			}

			MWCloseChannel(hChannel);
			hChannel = NULL;
		}
	}

	nItemCount = pSubMenu->GetMenuItemCount();
	m_bSelectAudio = new bool[nItemCount];
	memset(m_bSelectAudio, false, nItemCount);

	int nSelectDeviceAudioType = m_audioType;
	CString audioStr;
	CString str;

	if (nSelectDeviceAudioType == -1)
	{
		HCHANNEL hChannel = m_wndView.GetChannel();
		DWORD dwAudioInput = 0;
		MW_RESULT xr = MWGetAudioInputSource(hChannel, &dwAudioInput);
		if (xr == MW_SUCCEEDED)
		{
			switch (INPUT_TYPE(dwAudioInput))
			{
			case MWCAP_AUDIO_INPUT_TYPE_NONE:
				break;
			case MWCAP_AUDIO_INPUT_TYPE_HDMI:
			case MWCAP_AUDIO_INPUT_TYPE_SDI:
				audioStr.Format(_T("Digital"));
				break;
			case MWCAP_AUDIO_INPUT_TYPE_LINE_IN:
				audioStr.Format(_T("Analog"));
				break;
			case MWCAP_AUDIO_INPUT_TYPE_MIC_IN:
				audioStr.Format(_T("Mic"));
				break;
			}
		}

		USES_CONVERSION;
		int nItemCount = pSubMenu->GetMenuItemCount();
		for (int i = 0; i < nItemCount; i++)
		{
			pSubMenu->GetMenuStringW(ID_AUDIO_BEGIN + 1 + i, str, NULL);
			if (strstr(W2A(str), "Pro Capture") != NULL)
			{
				m_bSelectAudio[0] = true;
				break;
			}
			else if (strstr(W2A(str), W2A(audioStr)) != NULL)
			{
				m_bSelectAudio[i] = true;
				break;
			}
		}
	}

	BOOL bRet = m_wndView.OpenPreview(nChannel, m_audioType);
	if (bRet)
	{
		m_nIndex = nChannel;
	}
}

void CMainFrame::OnUpdateDeviceItem(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID == ID_DEVICE_BEGIN + 1 + m_nIndex)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
}

void CMainFrame::OnAudioItem(UINT nID)
{
	m_wndView.ClosePreview();
	m_audioType = DEFAULT;

	CMenu* pMenu = GetMenu();
	CMenu* pSubMenu = pMenu->GetSubMenu(2);
	CString str;
	pSubMenu->GetMenuStringW(nID, str, NULL);

	USES_CONVERSION;
	if (strstr(W2A(str), "Digital") != NULL || strstr(W2A(str), "Pro Capture") != NULL)
	{
		m_audioType = EMBEDDED;
	}
	else if (strstr(W2A(str), "Mic") != NULL)
	{
		m_audioType = MICROPHONE;
	}
	else if (strstr(W2A(str), "Computer") != NULL)
	{
		m_audioType = USB_CAPTURE;
	}
	else if (strstr(W2A(str), "Analog") != NULL)
	{
		m_audioType = LINE_IN;
	}

	int nCheckIndex = nID - ID_AUDIO_BEGIN - 1;
	if (m_bSelectAudio != NULL)
	{
		m_bSelectAudio[nCheckIndex] = !m_bSelectAudio[nCheckIndex];
	}

	int nItemCount = pSubMenu->GetMenuItemCount();
	int nSelectAudio = 0;
	for (int i = 0; i < nItemCount; i++)
	{
		if (m_bSelectAudio[i] == true)
			nSelectAudio++;
		if (nSelectAudio > 1)
		{
			memset(m_bSelectAudio, false, nItemCount);
			m_bSelectAudio[nCheckIndex] = true;
			break;
		}
	}

	BOOL bRet = m_wndView.OpenPreview(m_nIndex, m_audioType, m_bSelectAudio[nCheckIndex]);
	if (!bRet)
	{
		m_wndView.ClosePreview();
	}
}

void CMainFrame::OnUpdateAudioItem(CCmdUI *pCmdUI)
{
	int nItemNum = pCmdUI->m_nID - ID_AUDIO_BEGIN - 1;
	pCmdUI->SetCheck(m_bSelectAudio[nItemNum]);
}


BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnAppExit()
{
	m_wndView.ClosePreview();
	PostMessage(WM_QUIT, 0, 0);
}


void CMainFrame::OnClose()
{
	KillTimer(1);
	m_wndView.ClosePreview();
	CFrameWnd::OnClose();
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent){
	if(nIDEvent==1){
		int cx,cy;
		m_wndView.GetVideoFormat(cx,cy);

		CString strInfo;
		strInfo.Format(_T("%d x %d YUY2 %.02lf FPS"),cx,cy,m_wndView.GetPreviewFPS());
		m_wndStatusBar.SetWindowText(strInfo);
	}
}
