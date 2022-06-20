/************************************************************************************************/
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

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#include "CaptureThread.h"

// CSettingDlg dialog

class CSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CSettingDlg)

public:
	CSettingDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSettingDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int		m_cx;
	int		m_cy;
	DWORD	m_dwFrameDuration;

	int		m_nLayoutX;
	int		m_nLayoutY;

	int		m_nNumChannel;
	CHANNEL_INDEX m_arrChannel[MAX_CHANNEL_COUNT];

	void InitListCtrl(void);
	void RefreshStatus();
	int GetCheckedCount();

protected:
	bool m_bInited;

public:
	virtual BOOL OnInitDialog();
	void OnBnClickedOk();
	afx_msg void OnLvnItemchangedListChannels(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeLayout();

	CComboBox m_cmbLayout;
	CComboBox m_cmbResolution;
	CComboBox m_cmbFrameDuration;
	CListCtrl m_lstChannel;
	BOOL m_bSync;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
