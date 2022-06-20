/************************************************************************************************/
// SettingDlg.cpp : implementation file

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
#include "SettingDlg.h"
#include "afxdialogex.h"

#include "StringUtils.h"

// CSettingDlg dialog

typedef struct tagRESOLUTION_DATA {
	int nWidth;
	int nHeight;
} RESOLUTION_DATA;

const RESOLUTION_DATA g_arrResolution[] = {{720, 576},
					   {1280, 720},
					   {1920, 1080},
					   {2048, 1080}};

typedef struct tagLAYOUT_DATA {
	int x;
	int y;
} LAYOUT_DATA;

const LAYOUT_DATA g_arrLayout[] = {{1, 1}, {2, 1}, {2, 2}};

const DWORD g_arrFrameDuration[] = {400000, 333667, 333333, 166833, 166667};

IMPLEMENT_DYNAMIC(CSettingDlg, CDialog)

CSettingDlg::CSettingDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CSettingDlg::IDD, pParent), m_bSync(FALSE)
{
}

CSettingDlg::~CSettingDlg() {}

void CSettingDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LAYOUT, m_cmbLayout);
	DDX_Control(pDX, IDC_RESOLUTIONS, m_cmbResolution);
	DDX_Control(pDX, IDC_FRAMERATE, m_cmbFrameDuration);
	DDX_Control(pDX, IDC_LIST_CHANNELS, m_lstChannel);
	DDX_Check(pDX, IDC_CHECK_SYNC, m_bSync);
}

BEGIN_MESSAGE_MAP(CSettingDlg, CDialog)
ON_BN_CLICKED(IDOK, &CSettingDlg::OnBnClickedOk)
ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CHANNELS,
	  &CSettingDlg::OnLvnItemchangedListChannels)
ON_CBN_SELCHANGE(IDC_LAYOUT, &CSettingDlg::OnCbnSelchangeLayout)
END_MESSAGE_MAP()

// CSettingDlg message handlers

BOOL CSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bInited = false;
	// TODO:  Add extra initialization here
	CString strName;
	int nNumResolution =
		sizeof(g_arrResolution) / sizeof(g_arrResolution[0]);
	for (int i = 0; i < nNumResolution; i++) {
		strName.Format(_T("%d x %d"), g_arrResolution[i].nWidth,
			       g_arrResolution[i].nHeight);
		int nItem = m_cmbResolution.AddString(strName);
		if (m_cx == g_arrResolution[i].nWidth &&
		    m_cy == g_arrResolution[i].nHeight)
			m_cmbResolution.SetCurSel(nItem);
	}

	int nNumLayout = sizeof(g_arrLayout) / sizeof(g_arrLayout[0]);
	for (int i = 0; i < nNumLayout; i++) {
		strName.Format(_T("%d x %d"), g_arrLayout[i].x,
			       g_arrLayout[i].y);
		int nItem = m_cmbLayout.AddString(strName);
		if (m_nLayoutX == g_arrLayout[i].x &&
		    m_nLayoutY == g_arrLayout[i].y)
			m_cmbLayout.SetCurSel(nItem);
	}

	int nNumFrameDuration =
		sizeof(g_arrFrameDuration) / sizeof(g_arrFrameDuration[0]);
	for (int i = 0; i < nNumFrameDuration; i++) {
		strName.Format(_T("%.02f"),
			       (double)10000000 / g_arrFrameDuration[i]);
		int nItem = m_cmbFrameDuration.AddString(strName);
		if (m_dwFrameDuration == g_arrFrameDuration[i])
			m_cmbFrameDuration.SetCurSel(nItem);
	}

	m_lstChannel.InsertColumn(0, L"Devices", LVCFMT_CENTER, 200);
	m_lstChannel.InsertColumn(1, L"Status", LVCFMT_LEFT, 240);

	DWORD dwExtStyle = m_lstChannel.GetExtendedStyle();
	m_lstChannel.SetExtendedStyle(dwExtStyle | LVS_EX_FULLROWSELECT |
				      LVS_EX_CHECKBOXES);

	InitListCtrl();

	RefreshStatus();

	CButton *pButtton = (CButton *)GetDlgItem(IDOK);
	pButtton->SetFocus();

	return FALSE; // return TRUE unless you set the focus to a control
		      // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingDlg::InitListCtrl(void)
{
	m_lstChannel.DeleteAllItems();

	for (int i = 0; i < g_nValidChannelCount; i++) {
		MWCAP_CHANNEL_INFO mci = {0};
		MWGetChannelInfoByIndex(g_nValidChannel[i], &mci);
		CAutoConvertString strProductName(mci.szProductName);
		CString strName;
		strName.Format(_T("%02d-%d %s"), mci.byBoardIndex,
			       mci.byChannelIndex,
			       (const TCHAR *)strProductName);
		m_lstChannel.InsertItem(i, strName);

		BOOL bFind = FALSE;
		{
			for (int j = 0; j < m_nNumChannel; j++) {
				//if (m_arrChannel[j].nBoard == mci.byBoardIndex && m_arrChannel[j].nChannel == mci.byChannelIndex) {
				if (m_arrChannel[j].nIndex == i) {
					bFind = TRUE;
					break;
				}
			}
		}

		LVITEM lv = {0};
		lv.mask = LVIF_STATE;
		lv.iItem = i;
		lv.stateMask = LVIS_STATEIMAGEMASK;
		lv.state = INDEXTOSTATEIMAGEMASK(bFind ? 2 : 1);

		if (i == g_nValidChannelCount - 1)
			m_bInited = true;
		m_lstChannel.SetItem(&lv);

		WCHAR szDevicePath[MAX_PATH] = {0};
		MWGetDevicePath(g_nValidChannel[i], szDevicePath);

		CString strStatus = _T("---");
		HCHANNEL hChannel = MWOpenChannelByPath(szDevicePath);
		if (hChannel != NULL) {
			MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
			if (MW_SUCCEEDED ==
			    MWGetVideoSignalStatus(hChannel,
						   &videoSignalStatus)) {
				if (videoSignalStatus.state ==
				    MWCAP_VIDEO_SIGNAL_LOCKED) {
					strStatus.Format(_T("Locked, %d x %d"),
							 videoSignalStatus.cx,
							 videoSignalStatus.cy);
				} else {
					strStatus.Format(_T("Unlocked"));
				}
			}
			MWCloseChannel(hChannel);
		}
		m_lstChannel.SetItemText(i, 1, strStatus);
	}
}

void CSettingDlg::OnBnClickedOk()
{
	UpdateData();

	int nSel = m_cmbResolution.GetCurSel();
	m_cx = g_arrResolution[nSel].nWidth;
	m_cy = g_arrResolution[nSel].nHeight;

	nSel = m_cmbFrameDuration.GetCurSel();
	m_dwFrameDuration = g_arrFrameDuration[nSel];

	nSel = m_cmbLayout.GetCurSel();
	m_nLayoutX = g_arrLayout[nSel].x;
	m_nLayoutY = g_arrLayout[nSel].y;

	m_nNumChannel = 0;

	int nItem = m_lstChannel.GetItemCount();
	for (int i = 0; i < nItem; i++) {
		int nState = m_lstChannel.GetItemState(i, LVIS_STATEIMAGEMASK);
		int nImage = (nState >> 12);
		MWCAP_CHANNEL_INFO mci = {0};
		MWGetChannelInfoByIndex(i, &mci);
		if (nImage == 2) {
			m_arrChannel[m_nNumChannel].nIndex = i;
			m_arrChannel[m_nNumChannel].nBoard = mci.byBoardIndex;
			m_arrChannel[m_nNumChannel].nChannel =
				mci.byChannelIndex;

			m_nNumChannel++;
		}
	}

	CDialog::OnOK();
}

void CSettingDlg::RefreshStatus()
{
	int nSel = m_cmbLayout.GetCurSel();
	int nMaxCount = g_arrLayout[nSel].x * g_arrLayout[nSel].y;

	int nCheckCount = GetCheckedCount();
	int nItem = m_lstChannel.GetItemCount();
	if (nCheckCount < nMaxCount) {
		for (int i = 0; i < nItem; i++) {
			int nState = m_lstChannel.GetItemState(
				i, LVIS_STATEIMAGEMASK);
			int nImage = (nState >> 12);
			if (nImage == 0) {
				m_lstChannel.SetItemState(
					i, INDEXTOSTATEIMAGEMASK(1),
					LVIS_STATEIMAGEMASK);
			}
		}
	} else if (nCheckCount == nMaxCount) {
		for (int i = 0; i < nItem; i++) {
			int nState = m_lstChannel.GetItemState(
				i, LVIS_STATEIMAGEMASK);
			int nImage = (nState >> 12);
			if (nImage == 1) {
				m_lstChannel.SetItemState(
					i, INDEXTOSTATEIMAGEMASK(0),
					LVIS_STATEIMAGEMASK);
			}
		}
	} else if (nCheckCount > nMaxCount) {
		int nCurrentCheck = 0;
		for (int i = 0; i < nItem; i++) {
			int nState = m_lstChannel.GetItemState(
				i, LVIS_STATEIMAGEMASK);
			int nImage = (nState >> 12);
			if (nImage == 1) {
				m_lstChannel.SetItemState(
					i, INDEXTOSTATEIMAGEMASK(0),
					LVIS_STATEIMAGEMASK);
			} else if (nImage == 2) {
				if (nCurrentCheck == nMaxCount) {
					m_lstChannel.SetItemState(
						i, INDEXTOSTATEIMAGEMASK(0),
						LVIS_STATEIMAGEMASK);
					//nCheckCount --;
				} else {
					nCurrentCheck++;
				}
			}
		}
		nCheckCount = nCurrentCheck;
	}
	m_lstChannel.Invalidate();

	GetDlgItem(IDC_CHECK_SYNC)->EnableWindow(nCheckCount > 1);
	UpdateData(TRUE);
	if (m_bInited)
		if (nCheckCount <= 1 && m_bSync) {
			m_bSync = FALSE;
			UpdateData(FALSE);
		}
}

void CSettingDlg::OnLvnItemchangedListChannels(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	RefreshStatus();

	*pResult = 0;
}

int CSettingDlg::GetCheckedCount()
{
	int nCheckCount = 0;
	int nItem = m_lstChannel.GetItemCount();
	for (int i = 0; i < nItem; i++) {
		int nState = m_lstChannel.GetItemState(i, LVIS_STATEIMAGEMASK);
		int nImage = (nState >> 12);
		if (nImage == 2) {
			nCheckCount++;
		}
	}
	return nCheckCount;
}

void CSettingDlg::OnCbnSelchangeLayout()
{
	RefreshStatus();
}

BOOL CSettingDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN))

		return TRUE;

	return CDialog::PreTranslateMessage(pMsg);
}
