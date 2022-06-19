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
#pragma once

#include <Windows.h>

class IPreviewCallback
{
public:
	virtual BOOL OnVideoCallback(const BYTE* lpData, DWORD cbStride, BOOL bSignalValid) = NULL;
	virtual BOOL OnAudioCallback(const BYTE* lpData, int cbSize, DWORD dwSamplerate, BOOL bSignalValid) = NULL;
	virtual void OnVideoSignalChanged(int cx, int cy, BOOL bSignalValid) = NULL;
	virtual void OnAudioSignalChanged(BOOL bValid, BOOL bLPCM, BYTE cBitsPerSample, DWORD dwSampleRate, BOOL bSignalValid) = NULL;
};

class CPreviewThread
{
public:
	CPreviewThread(void);
	~CPreviewThread(void);

public:
	BOOL Create(int nIndex, int cx, int cy, DWORD dwFrameDuration, DWORD dwFOURCC, BOOL bPreviewFlag, IPreviewCallback* pCallback);
	void Destroy();

	static DWORD WINAPI VideoCaptureThreadProc(LPVOID pvParam) {
		CPreviewThread* pThis = (CPreviewThread *)pvParam;
		return pThis->VideoCaptureThreadProcEx();
	}
	DWORD VideoCaptureThreadProcEx();

	static DWORD WINAPI AudioCaptureThreadProc(LPVOID pvParam) {
		CPreviewThread* pThis = (CPreviewThread *)pvParam;
		return pThis->AudioCaptureThreadProcEx();
	}
	DWORD AudioCaptureThreadProcEx();

	BOOL GetInputAudioFormat(BOOL& bLPCM, BYTE& cBitsPerSample, DWORD& dwSampleRate) {
		if (m_bAudioValid) {
			bLPCM			= m_bIsLPCM;
			cBitsPerSample	= m_cBitsPerSample;
			dwSampleRate	= m_dwSampleRate;
			return TRUE;
		}
		return FALSE;
	}

protected:
	HCHANNEL		m_hChannel;
	//video
	int				m_cx;
	int				m_cy;
	HANDLE			m_hThread;
	HANDLE			m_hEvent;
	DWORD			m_dwFrameDuration;
	DWORD			m_dwFOURCC;

	MWCAP_VIDEO_SIGNAL_STATUS	m_status;

	//audio
	int				m_nPresentChannel;
	int				m_nBitDepthInByte;
	BYTE			m_cBitsPerSample;
	DWORD			m_dwSampleRate;
	BOOL			m_bPreviewFlag;
	BOOL			m_bIsLPCM;
	BOOL			m_bAudioValid;
	BOOL			m_bAudioCapturing;
	HANDLE			m_hAudioEvent;
	HANDLE			m_hAudioCaptureThread;
	
	MWCAP_AUDIO_CAPTURE_FRAME		m_audioFrame;
	MWCAP_AUDIO_SIGNAL_STATUS		m_audioSignalStatus;
	//BYTE							m_byAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * MAX_BIT_DEPTH_IN_BYTE];
	short							m_byAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * 8];
	short							m_byAudioSamplesPreview[MWCAP_AUDIO_SAMPLES_PER_FRAME * 2];
	
	IPreviewCallback* m_pCallback;
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;