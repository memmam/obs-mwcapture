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

// CChildView window
#include "CaptureThread.h"
#include "DSoundRenderer.h"
#include "MWDXRenderThread.h"
#include "Mp4FileSave.h"
#include <queue>
#include <list>
using namespace std;

typedef struct mw_disk_size_s{
	uint32_t		m_u32_gb;
	uint32_t		m_u32_mb;
	uint32_t		m_u32_kb;
	uint32_t		m_u32_b;
}mw_disk_size_t;


class CChildView : public CWnd, public ICaptureCallback
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	BOOL GetVideoFormat(int& cx, int& cy,MWCAP_VIDEO_COLOR_FORMAT &colorFormat, MWCAP_VIDEO_QUANTIZATION_RANGE &quantRange) {
		return m_previewThread.GetCaptureVideoFormat(cx, cy, colorFormat, quantRange);
	}
	double GetPreviewFPS() {
		return m_previewThread.GetFPS();
	}
	double GetRecordFPS() {
		return m_recordThread.GetFPS();
	}
	double GetAudioSPS() {
		return m_recordThread.GetSPS();
	}

	typedef enum {
		PREVIEW_THREAD = 0x01,
		RECORD_THREAD
	};

	// ICaptureCallback
	void OnVideoSignalChanged(int cx, int cy, DWORD dwParam);
	void OnCaptureCallback(const BYTE* pbyData, int cbStride, DWORD dwParam);
	void OnAudioSignalChanged(BOOL bChannelValid, BOOL bPCM, BYTE cBitsPerSample, DWORD dwSampleRate, DWORD dwParam);
	void OnAudioCallback(const BYTE* pbyData, int cbSize, uint64_t u64TimeStamp,DWORD dwParam);
	// Generated message map functions

	vector<mw_venc_gpu_info_t> GetGPUInfos();
protected:
	BOOL OpenPreview(int nIndex);
	void ClosePreview();

	static DWORD WINAPI EncodeVideoFrameThread(LPVOID pParam)
	{
		CChildView* pThis = (CChildView*)pParam;
		return pThis->EncodeVideoFrameThreadProc();
	}

	DWORD EncodeVideoFrameThreadProc();

	BOOL get_disk_size(mw_disk_size_t& t_size);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	afx_msg void OnDeviceItem(UINT nID);
	afx_msg void OnUpdateDeviceItem(CCmdUI *pCmdUI);

	afx_msg void OnFileRecordItem(UINT nID);
	afx_msg void OnUpdateFileRecordItem(CCmdUI *pCmdUI);

	afx_msg LRESULT OnMsgVideoSignalChanged(WPARAM w, LPARAM l);
	afx_msg LRESULT OnMsgVideoRenderReset(WPARAM w, LPARAM l);
	afx_msg UINT OnPowerBroadcast(UINT nPowerEvent, UINT nEventData);
	afx_msg LRESULT OnMsgDiskSizeOver(WPARAM w, LPARAM l);
	
protected:
	CCaptureThread			m_previewThread;
	CCaptureThread			m_recordThread;

	HANDLE					m_hChannel;
	
	CMWDXRenderThread*		m_p_VideoRenderThread;
	CMWRenderBuffer*		m_p_RenderBuffer;

	HWND					m_hwnd;
	CDSoundRenderer*		m_pAudioRenderer;

	int						m_nChannel;

	HANDLE					m_hPowerNotify;

	CMWLock					m_lockVideo;
	CMWLock					m_lockAudio;

	int						m_nClip;

	CMp4FileSave			m_FileRecord;
	BOOL					m_bRecord;
	int						m_bufWidth;
	int						m_bufHeight;

	char					m_cs_dir[256];
	WCHAR					m_wcs_dir[256];

	CRITICAL_SECTION		m_cs;
	HANDLE					m_hEncodeThread;
	HANDLE					m_hEncodeFrame;
	HANDLE					m_hExitEncodeThread;
	int						m_n_id;
	DWORD					m_dwCaptureCount;

	uint32_t				m_u32Record;
public:
	afx_msg void OnClose();
	afx_msg void OnFileNoclip();
	afx_msg void OnClipTopleft();
	afx_msg void OnClipTopright();
	afx_msg void OnUpdateFileNoclip(CCmdUI *pCmdUI);
	afx_msg void OnUpdateClipTopleft(CCmdUI *pCmdUI);
	afx_msg void OnUpdateClipTopright(CCmdUI *pCmdUI);
	afx_msg void OnClipBottomleft();
	afx_msg void OnUpdateClipBottomleft(CCmdUI *pCmdUI);
	afx_msg void OnClipBottomright();
	afx_msg void OnUpdateClipBottomright(CCmdUI *pCmdUI);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnFileStoprecord();
	afx_msg void OnUpdateFileStoprecord(CCmdUI *pCmdUI);
};

