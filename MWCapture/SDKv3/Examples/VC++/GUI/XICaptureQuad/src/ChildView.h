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

#include "MWFOURCC.h"

#include "CaptureThread.h"
#include "MWDXRender.h"

// CChildView window

class CChildView : public CWnd, public ICaptureCallback {
	// Construction
public:
	CChildView();

	// Attributes
public:
	// Operations
public:
	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	// Implementation
public:
	virtual ~CChildView();

	double GetCurrentFPS() { return m_thread.GetFPS(); }
	int GetValidChannel() { return m_thread.GetValidChannel(); }

	// ICaptureCallback
	void OnVideoSignalChanged(int cx, int cy, DWORD dwParam);
	void OnCaptureCallback(const BYTE *pbyData, int cbStride,
			       DWORD dwParam);

	// Generated message map functions
protected:
	BOOL OpenPreview();
	void ClosePreview();

	INT_PTR CallFileSettings();

	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, UINT nEventData);
	afx_msg void OnFileSettings();

protected:
	MWDXRender *m_pVideoRenderer;
	HPOWERNOTIFY m_hPowerNotify;

	CCaptureThread m_thread;

	int m_nLayoutX;
	int m_nLayoutY;

	CMWLock m_lock;

	int m_cx;
	int m_cy;
	DWORD m_dwFrameDuration;
	DWORD m_dwFourcc;

	int m_nNumChannel;
	CHANNEL_INDEX m_arrChannel[MAX_CHANNEL_COUNT];

	BOOL m_bSyncMode;

public:
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};
