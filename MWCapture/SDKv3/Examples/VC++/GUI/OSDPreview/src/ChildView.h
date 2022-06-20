/************************************************************************************************/
// ChildView.h : interface of the CChildView class

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

#include "stdafx.h"
#include "MWDXRender.h"

// CChildView window

class CChildView : public CWnd {
	// Construction
public:
	CChildView();

	// Attributes
public:
	HANDLE m_hThread;
	HANDLE m_hExitThread;

	HCHANNEL m_hChannel;
	MWDXRender *m_pRenderer;

	BOOL m_bBorder;
	BOOL m_bText;
	BOOL m_bIcon;

	Bitmap *m_pIcon[5];
	int m_nIconIndex;

	DWORD m_dwFrameCount;

	MWCAP_VIDEO_COLOR_FORMAT m_colorFormat;
	MWCAP_VIDEO_QUANTIZATION_RANGE m_quantRange;

	// Operations
public:
	BOOL SelectChannel(int nIndex);

	static DWORD WINAPI ThreadProc(LPVOID pvParam)
	{
		CChildView *pThis = (CChildView *)pvParam;
		return pThis->ThreadProc();
	}
	DWORD ThreadProc();

	BOOL LoadOSD(HOSD hOSD);
	BOOL LoadText(HOSD hOSD);

	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	// Implementation
public:
	virtual ~CChildView();

	BOOL LoadImageFromResource(Bitmap **ppImage, UINT nResID,
				   LPCTSTR lpTyp);
	// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileText();
	afx_msg void OnUpdateFileText(CCmdUI *pCmdUI);
	afx_msg void OnFileBorder();
	afx_msg void OnUpdateFileBorder(CCmdUI *pCmdUI);
	afx_msg void OnFileIcon();
	afx_msg void OnUpdateFileIcon(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;
