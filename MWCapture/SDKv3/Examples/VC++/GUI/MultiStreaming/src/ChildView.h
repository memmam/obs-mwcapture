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

#define FRAME_DURATION 333333

#define CAPTURE_COLORSPACE MWFOURCC_RGBA

// CChildView window

class CChildView : public CView {
	// Construction
public:
	CChildView();
	virtual ~CChildView();

	// Attributes
public:
	DECLARE_DYNCREATE(CChildView)
	// Operations
public:
	// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT &cs);

	// Implementation
public:
	BOOL StartPreview(int nChannelId, int cx, int cy);
	void StopPreview();

	BOOL GetCaptureInfo(int *pcx, int *pcy, DWORD *pdwFmt, double *pdFps)
	{
		if (m_hChannel == NULL)
			return FALSE;

		*pcx = m_nWidth;
		*pcy = m_nHeight;
		*pdwFmt = CAPTURE_COLORSPACE;
		*pdFps = m_dCurrentFps;

		return TRUE;
	}

public:
	virtual void OnDraw(CDC * /*pDC*/);
	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	static DWORD WINAPI VideoThreadProc(LPVOID pvParam)
	{
		CChildView *pThis = (CChildView *)pvParam;
		return pThis->VideoThreadProcEx();
	}
	DWORD VideoThreadProcEx();

private:
	MWDXRender *m_pRenderer;
	HCHANNEL m_hChannel;

	HANDLE m_hExitEvent;
	HANDLE m_hCaptureThread;

	double m_dCurrentFps;

	int m_nWidth;
	int m_nHeight;

public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;