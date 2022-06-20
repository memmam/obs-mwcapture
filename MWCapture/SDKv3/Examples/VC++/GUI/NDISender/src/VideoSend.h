/************************************************************************************************/
// NDISenderDlg.h : Header File

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
#include "PreviewThread.h"
#include "LockUtils.h"

#define SEND_WIDTH		1920
#define SEND_HEIGHT		1080
#define SEND_DURATION	333334
#define NDIFOURCC		NDIlib_FourCC_type_BGRA
#define SEND_FOURCC		MWFOURCC_ARGB

class CVideoSend :public CWnd
{
	DECLARE_DYNAMIC(CVideoSend)
public:
	CVideoSend(void);
	~CVideoSend(void);

public:

	BOOL CreateNDISender(int nIndex, int cx, int cy, DWORD dwFrameDuration, DWORD dwFourcc, char* pSenderName);
	void DestoryNDISender();
	BOOL GetNDIVar(NDIlib_send_instance_t *pNDI_send, NDIlib_video_frame_v2_t *pVideoFrame, NDIlib_audio_frame_interleaved_16s_t *pAudioFrame);

protected:
	BOOL SetNDICaptureFrameParams(int cx, int cy, DWORD dwFrameDuration, DWORD dwFourcc);
protected:
	CMWLock				m_lock;
	CMWLock				m_lockRecrete;

	//NDI
	NDIlib_send_instance_t					m_pNDI_send;
	NDIlib_video_frame_v2_t					m_videoFrame;
	NDIlib_audio_frame_interleaved_16s_t	m_audioFrame;

	//capture
	int		m_nIndex;
	int		m_cx;
	int		m_cy;
	DWORD   m_dwFrameDuration;
	DWORD   m_dwFourcc;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg LRESULT OnMsgVideoSignalChanged(WPARAM w, LPARAM l);

};

