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
#include <MMSystem.h>

#include "MWFOURCC.h"
#include "LockUtils.h"
#include "DSoundRenderer.h"
#include "MWDXRender.h"


#define PREVIEW_WIDTH		1920
#define PREVIEW_HEIGHT		1080


#define PREVIEW_FOURCC		MWFOURCC_YUY2 // MWFOURCC_BGRA // MWFOURCC_RGBA // MWFOURCC_YUY2
#define PREVIEW_DURATION	400000


typedef enum _AudioType
{
	DEFAULT = -1,
	EMBEDDED,
	MICROPHONE,
	USB_CAPTURE,
	LINE_IN,
}AudioType;

// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();
	// Implementation
public:
	virtual ~CChildView();
// Attributes
public:

// Operations
public:
	BOOL GetVideoFormat(int &cx,int &cy){
		cx=m_n_width;
		cy=m_n_height;
		return TRUE;
	}

	double GetPreviewFPS(){
		return m_d_fps;
	}

	BOOL OpenPreview(int nIndex, AudioType audioType, bool audioRender = true);
	void ClosePreview();
	HCHANNEL GetChannel();

// Overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	static void OnVideoCaptureCallback(BYTE *pbFrame, int cbFrame, UINT64 u64TimeStamp, void* pParam)
	{
		CChildView* pThis = (CChildView*)pParam;
		pThis->OnVideoCaptureCallbackProc(pbFrame, cbFrame, u64TimeStamp);
	}

	void OnVideoCaptureCallbackProc(BYTE *pbFrame, int cbFrame, UINT64 u64TimeStamp)
	{
		CMWAutoLock lock(m_lockVideo);

		if(m_pDX11VideoRenderer!=NULL){
			m_pDX11VideoRenderer->paint((unsigned char*)pbFrame);
		}
	}
	
	static void OnAudioCaptureCallback(const BYTE * pbFrame, int cbFrame, UINT64 u64TimeStamp, void* pParam)
	{
		CChildView* pThis = (CChildView*)pParam;
		pThis->OnAudioCaptureCallbackProc(pbFrame, cbFrame, u64TimeStamp);
	}

	void OnAudioCaptureCallbackProc(const BYTE * pbFrame, int cbFrame, UINT64 u64TimeStamp)
	{
		CMWAutoLock lock(m_lockAudio);
		if (m_pAudioRenderer) {
			BOOL bRet = m_pAudioRenderer->PutFrame(pbFrame, cbFrame);
		}
	}

	WCHAR * charToWchar(const char *s){

		int w_nlen=MultiByteToWideChar(CP_ACP,0,s,-1,NULL,0);

		WCHAR *ret;

		ret=(WCHAR*) malloc(sizeof(WCHAR)*w_nlen);

		memset(ret,0,sizeof(ret));

		MultiByteToWideChar(CP_ACP,0,s,-1,ret,w_nlen);

		return ret;

	}

	void BGR2RGB(BYTE *pBGR, BYTE *pRGB, DWORD dwWidth, DWORD dwHeight)
	{
		for(int i = 0; i < dwWidth; i++)
		{
			for(int j = 0; j < dwHeight; j++)
			{
				*(pRGB + ((dwHeight - j - 1) * (dwWidth * 4)) + (i * 4 + 0)) = *(pBGR + j * (dwWidth * 4) + (i * 4 + 0));
				*(pRGB + ((dwHeight - j - 1) * (dwWidth * 4)) + (i * 4 + 1)) = *(pBGR + j * (dwWidth * 4) + (i * 4 + 1));
				*(pRGB + ((dwHeight - j - 1) * (dwWidth * 4)) + (i * 4 + 2)) = *(pBGR + j * (dwWidth * 4) + (i * 4 + 2));
				*(pRGB + ((dwHeight - j - 1) * (dwWidth * 4)) + (i * 4 + 3)) = *(pBGR + j * (dwWidth * 4) + (i * 4 + 3));
			}
		}
	}

protected:

	HCHANNEL		m_hChannel;

	HANDLE			m_hVideo;
	HANDLE			m_hAudio;

	CMWLock			m_lockVideo;
	CMWLock			m_lockAudio;

	MWDXRender *m_pDX11VideoRenderer;
	CDSoundRenderer *m_pAudioRenderer;

	BYTE*			m_pBuf;

	int				m_n_width;
	int				m_n_height;
	double			m_d_fps;

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

