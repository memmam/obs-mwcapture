/************************************************************************************************/
// StatusView.cpp : implementation of the CStatusView class

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
#include "StatusView.h"

const TCHAR *GetColorFormatName(DWORD fmt)
{
	switch (fmt) {
	case MWFOURCC_UNK:
		return _T("Unknown");
	case MWFOURCC_RGB24:
		return _T("RGB24");
	case MWFOURCC_RGBA:
		return _T("RGB32");
	case MWFOURCC_BGR24:
		return _T("BGR24");
	case MWFOURCC_BGRA:
		return _T("BGR32");
	case MWFOURCC_YUYV:
		return _T("YUYV");
	case MWFOURCC_YVYU:
		return _T("YVYU");
	case MWFOURCC_UYVY:
		return _T("UYVY");
	case MWFOURCC_VYUY:
		return _T("VYUY");
	case MWFOURCC_I420:
		return _T("I420");
	case MWFOURCC_NV12:
		return _T("NV12");
	default:
		return _T("Unknown");
	}
}

// CStatusView

IMPLEMENT_DYNCREATE(CStatusView, CView)

CStatusView::CStatusView()
{
	m_strStatus = _T("--");
}

CStatusView::~CStatusView() {}

BEGIN_MESSAGE_MAP(CStatusView, CView)
ON_WM_PAINT()
END_MESSAGE_MAP()

// CStatusView 绘图

void CStatusView::OnDraw(CDC *pDC)
{
	CDocument *pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}

// CStatusView 诊断

#ifdef _DEBUG
void CStatusView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CStatusView::Dump(CDumpContext &dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG

// CStatusView 消息处理程序

void CStatusView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	do {
		CDC *pDC = GetDC();
		if (pDC == NULL)
			break;

		CFont *font = GetParent()->GetFont();

		HANDLE hOldObject = dc.SelectObject(font);

		pDC->TextOut(0, 0, m_strStatus);

		dc.SelectObject(hOldObject);

		ReleaseDC(pDC);

	} while (FALSE);
	// Do not call CWnd::OnPaint() for painting messages
}

void CStatusView::UpdateStatus(int cx, int cy, DWORD dwFmt, double dFps)
{
	m_strStatus.Format(_T("%d x %d %s , %.02f FPS\0"), cx, cy,
			   GetColorFormatName(dwFmt), dFps);
	Invalidate();
}
