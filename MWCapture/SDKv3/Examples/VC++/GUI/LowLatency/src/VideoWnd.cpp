/************************************************************************************************/
// VideoWnd.cpp : implementation file

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
#include "VideoWnd.h"

#define PREVIEW_WIDTH 352
#define PREVIEW_HEIGHT 240
#define PREVIEW_DURATION 333334
#define PREVIEW_FOURCC MWFOURCC_RGBA

// CVideoWnd
IMPLEMENT_DYNAMIC(CVideoWnd, CWnd)

CVideoWnd::CVideoWnd()
{
	m_pRenderer = NULL;
}

CVideoWnd::~CVideoWnd() {}

BEGIN_MESSAGE_MAP(CVideoWnd, CWnd)
ON_WM_ERASEBKGND()
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()
END_MESSAGE_MAP()

int CVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pRenderer = new MWDXRender();
	bool t_b_reverse = false;
	if (PREVIEW_FOURCC == MWFOURCC_BGRA || PREVIEW_FOURCC == MWFOURCC_BGR24)
		t_b_reverse = true;
	if (!m_pRenderer->initialize(PREVIEW_WIDTH, PREVIEW_HEIGHT,
				     PREVIEW_FOURCC, t_b_reverse, GetSafeHwnd(),
				     MW_CSP_BT_709, MW_CSP_LEVELS_TV,
				     MW_CSP_LEVELS_PC)) {
		delete m_pRenderer;
		m_pRenderer = NULL;
		return -1;
	}

	return 0;
}

void CVideoWnd::OnDestroy()
{
	if (m_pRenderer != NULL) {
		m_pRenderer->cleanup_device();
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	CWnd::OnDestroy();
}

BOOL CVideoWnd::OnEraseBkgnd(CDC *pDC)
{
	return CWnd::OnEraseBkgnd(pDC);
}

void CVideoWnd::OnPaint()
{
	CPaintDC dc(this); // device context for painting
}

BOOL CVideoWnd::OpenChannel(int nIndex)
{
	CloseChannel();

	BOOL bRet = FALSE;
	bRet = m_thread.Create(g_nValidChannel[nIndex], PREVIEW_WIDTH,
			       PREVIEW_HEIGHT, PREVIEW_DURATION, PREVIEW_FOURCC,
			       this);
	return bRet;
}

void CVideoWnd::CloseChannel()
{
	m_thread.Destroy();
}

BOOL CVideoWnd::OnVideoCallback(const BYTE *lpData, DWORD cbStride)
{
	if (m_pRenderer != NULL) {
		m_pRenderer->paint((unsigned char *)lpData);
	}
	return TRUE;
}
