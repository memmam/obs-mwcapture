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

#include <MMSystem.h>

#include "MWFOURCC.h"
#include "LockUtils.h"

#include "PreviewThread.h"
#include "DSoundRenderer.h"
#include "MWDXRender.h"

#include <queue>
using namespace std;

#define PREVIEW_WIDTH 1920
#define PREVIEW_HEIGHT 1080
#define PREVIEW_DURATION 333334

class CVideoWnd : public CWnd, public IPreviewCallback {
	DECLARE_DYNAMIC(CVideoWnd)

public:
	CVideoWnd(void);
	~CVideoWnd(void);

public:
	BOOL OpenChannel(int nIndex, int cx, int cy, DWORD dwFrameDuration,
			 DWORD dwFOURCC);
	void CloseChannel();

	// ICaptureCallback
	BOOL OnVideoCallback(const BYTE *lpData, DWORD cbStride,
			     BOOL bSignalValid);
	BOOL OnAudioCallback(const BYTE *lpData, int cbSize, DWORD dwSamplerate,
			     BOOL bSignalValid);

	void OnVideoSignalChanged(int cx, int cy, BOOL bSignalValid);
	void OnAudioSignalChanged(BOOL bValid, BOOL bLPCM, BYTE cBitsPerSample,
				  DWORD dwSampleRate, BOOL bSignalValid);

	static DWORD WINAPI NDISendThreadVideo(LPVOID pvParam)
	{
		CVideoWnd *pThis = (CVideoWnd *)pvParam;
		return pThis->NDISendThreadVideoProc();
	}

	DWORD NDISendThreadVideoProc();

	bool CreateNDISendThread();

	void DestoryNDISendThread();

	static DWORD WINAPI NDISendThreadAudio(LPVOID pvParam)
	{
		CVideoWnd *pThis = (CVideoWnd *)pvParam;
		return pThis->NDISendThreadAudioProc();
	}

	DWORD NDISendThreadAudioProc();

public:
	BOOL m_bNDISend;

protected:
	int m_cx;
	int m_cy;
	int m_nChannelIndex;
	DWORD m_dwFourcc;
	DWORD m_dwFrameDuration;

	MWDXRender *m_pRenderer;
	CDSoundRenderer *m_pAudioRenderer;
	CPreviewThread m_thread;
	CMWLock m_lockAudio;
	CMWLock m_lockVideo;

	HANDLE m_hNDIThreadVideo;
	HANDLE m_hEventSendVideo;
	HANDLE m_hEventExitVideo;
	bool m_b_running_video;

	HANDLE m_hNDIThreadAudio;
	HANDLE m_hEventSendAudio;
	HANDLE m_hEventExitAudio;
	bool m_b_running_audio;

	char m_cs_audio_0[7680];
	char m_cs_audio_1[7680];
	int m_n_index_audio;
	int m_n_samples;
	int m_n_pos;

	char **m_pa_data_video;
	int m_n_num_data_video;
	int m_n_size_data_video;
	queue<char *> m_queue_used_video;
	queue<char *> m_queue_send_video;
	CRITICAL_SECTION m_cs_send_video;

public:
	NDIlib_send_instance_t m_pNDI_send;
	NDIlib_video_frame_v2_t m_videoFrame;
	NDIlib_audio_frame_interleaved_16s_t m_audioFrame;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();

	afx_msg LRESULT OnMsgVideoSignalChanged(WPARAM w, LPARAM l);
};
