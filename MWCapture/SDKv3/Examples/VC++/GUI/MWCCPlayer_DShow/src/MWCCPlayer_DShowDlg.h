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

#include "MWCCGraphBuilder.h"

#pragma once

// CMWCCPlayer_DShowDlg dialog
class CMWCCPlayer_DShowDlg : public CDialogEx {
	// Construction
public:
	CMWCCPlayer_DShowDlg(CWnd *pParent = NULL); // standard constructor
	~CMWCCPlayer_DShowDlg();

	// Dialog Data
	enum { IDD = IDD_MWCCPLAYER_DSHOW_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

public:
	MWCCGraphBuilder *m_pCCGrapgBuilder;

protected:
	CRect m_Rect;

	// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

protected:
	CWnd *m_pcwndRadioCC;
	bool m_bCCChecked;
	CWnd *m_pcwndRadioWithoutCC;
	bool m_bWithoutCCChecked;
	CWnd *m_pcwndRadioSetCC;
	bool m_bSetCCChecked;

	CWnd *m_pcwndDeviceCombo;
	int m_nDevIndex;
	int m_ntDevIndex;

	CWnd *m_pcwndCheck708;
	CWnd *m_pcwndCheck608;
	bool m_bShow708;
	bool m_btShow708;
	bool m_bShow608;
	bool m_btShow608;

	CWnd *m_pcwndChannel608;
	int m_nCC608ChannelIndex;
	int m_ntCC608ChannelIndex;

	CWnd *m_pcwndFontFilePath;
	WCHAR m_wcsFontPath[256];
	WCHAR m_wcstFontPath[256];

	CWnd *m_pcwndFontOpen;

	CWnd *m_pcwndFontSize;
	int m_nFontRadio;
	int m_ntFontRadio;

	CWnd *m_pcwndBackCombo;
	int m_nBackSel;
	int m_ntBackSel;

	CWnd *m_pcwndBackR;
	int m_nBackR;
	int m_ntBackR;

	CWnd *m_pcwndBackG;
	int m_nBackG;
	int m_ntBackG;

	CWnd *m_pcwndBackB;
	int m_nBackB;
	int m_ntBackB;

	CWnd *m_pcwndBackA;
	int m_nBackA;
	int m_ntBackA;

	CWnd *m_pcwndForeComno;
	int m_nForeSel;
	int m_ntForeSel;

	CWnd *m_pcwndForeR;
	int m_nForeR;
	int m_ntForeR;

	CWnd *m_pcwndForeG;
	int m_nForeG;
	int m_ntForeG;

	CWnd *m_pcwndForeB;
	int m_nForeB;
	int m_ntForeB;

	CWnd *m_pcwndForeA;
	int m_nForeA;
	int m_ntForeA;

	CWnd *m_pcwndWidth;
	int m_nWith;
	int m_ntWidth;

	CWnd *m_pcwndHeight;
	int m_nHeight;
	int m_ntHeight;

	CWnd *m_pcwndButRun;
	CWnd *m_pcwndButPause;
	CWnd *m_pcwndButStop;

protected:
	void ChangeSize(CWnd *hwnd, int cx, int cy);

public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRadioWithCc();
	afx_msg void OnStnClickedStaticBackR2();
	afx_msg void OnBnClickedRadioWitoutCc();
	afx_msg void OnBnClickedButStop();
	afx_msg void OnBnClickedButPause();
	afx_msg void OnCbnSelchangeComboDevices();
	afx_msg void OnBnClickedButOk();
	afx_msg void OnBnClickedButCancel();
	afx_msg void OnBnClickedCheckCc708();
	afx_msg void OnBnClickedCheckCc608();
	afx_msg void OnCbnSelchangeComboCcChannel();
	afx_msg void OnBnClickedButOpen();
	afx_msg void OnEnChangeEditFontSize();
	afx_msg void OnCbnSelchangeComboBackColor();
	afx_msg void OnEnChangeEditBackR();
	afx_msg void OnEnChangeEditBackG();
	afx_msg void OnEnChangeEditBackB();
	afx_msg void OnEnChangeEditBackA();
	afx_msg void OnCbnSelchangeComboForeColor();
	afx_msg void OnEnChangeEditForeR();
	afx_msg void OnEnChangeEditForeG();
	afx_msg void OnEnChangeEditForeB();
	afx_msg void OnEnChangeEditForeA();
	afx_msg void OnEnChangeEditCcWidth();
	afx_msg void OnEnChangeEditCcHeight();

public:
	void FreshUIConnectCC(bool t_bConnectCC);
	void FreshUIConnect(bool t_bConnect);
	void ReviseCCSet();
	afx_msg void OnBnClickedRadioSetCc();
	afx_msg void OnEnKillfocusEditFontSize();
	afx_msg void OnEnKillfocusEditBackR();
	afx_msg void OnEnKillfocusEditBackG();
	afx_msg void OnEnKillfocusEditBackB();
	afx_msg void OnEnKillfocusEditBackA();
	afx_msg void OnEnKillfocusEditForeR();
	afx_msg void OnEnKillfocusEditForeG();
	afx_msg void OnEnKillfocusEditForeB();
	afx_msg void OnEnKillfocusEditForeA();
	afx_msg void OnEnKillfocusEditCcWidth();
	afx_msg void OnEnKillfocusEditCcHeight();
};
