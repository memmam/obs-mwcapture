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

#include "MWRenderBuffer.h"

class ICaptureCallback
{
public:
	virtual void OnVideoSignalChanged(int cx, int cy, DWORD dwParam) = NULL;
	virtual void OnCaptureCallback(const BYTE* pbyData, int cbStride, DWORD dwParam) = NULL;

	virtual void OnAudioSignalChanged(BOOL bValid, BOOL bLPCM, BYTE cBitsPerSample, DWORD dwSampleRate, DWORD dwParam) = NULL;
	virtual void OnAudioCallback(const BYTE* pbyData, int cbSize, uint64_t u64TimeStamp,DWORD dwParam) = NULL;
};

enum {
	CLIP_NONE,
	CLIP_TOPLEFT,
	CLIP_TOPRIGHT,
	CLIP_BOTTOMLEFT,
	CLIP_BOTTOMRIGHT,
};

class CCaptureThread
{
public:
	CCaptureThread();
	virtual ~CCaptureThread();

public:
	double GetFPS() {
		return m_dFramePerSecond;
	}
	double GetSPS() {
		return m_dSamplePerSecond;
	}
	BOOL GetCaptureVideoFormat(int& cx, int& cy, MWCAP_VIDEO_COLOR_FORMAT &colorFormat, MWCAP_VIDEO_QUANTIZATION_RANGE &quantRange) {
		cx = m_cx;
		cy = m_cy;
		colorFormat = m_colorFormat;
		quantRange = m_quantRange;
		return TRUE;
	}
	BOOL GetInputAudioFormat(BOOL& bLPCM, BYTE& cBitsPerSample, DWORD& dwSampleRate) {
		if (m_bAudioValid) {
			bLPCM			= m_bLPCM;
			cBitsPerSample	= m_cBitsPerSample;
			dwSampleRate	= m_dwSampleRate;
			return TRUE;
		}
		return FALSE;
	}

	BOOL Create(MWCAP_VIDEO_COLOR_FORMAT colorFormat, MWCAP_VIDEO_QUANTIZATION_RANGE quantRange,
		        int nChannel, int cx, int cy, 
		        DWORD dwFourcc, DWORD dwFrameDuration, ICaptureCallback* pCallback, 
		        BOOL bCaptureAudio, DWORD dwParam = 0, int nClip = CLIP_NONE);
	void Destroy();

	static DWORD WINAPI VideoThreadProc(LPVOID pvParam) {
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		MWCAP_CHANNEL_INFO mci;

		MWGetChannelInfo(pThis->m_hChannel, &mci);

		if (0 == strcmp(mci.szFamilyName, "Eco Capture")) 
		{
			return pThis->VideoEcoThreadProc();

		} else {
			return pThis->VideoProThreadProc();
		}
	}

	DWORD VideoProThreadProc();
	DWORD VideoEcoThreadProc();

	static DWORD WINAPI AudioThreadProc(LPVOID pvParam) {
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		return pThis->AudioThreadProc();
	}
	DWORD AudioThreadProc();

	DWORD GetAdjustFrameDuration();

	void SetRenderBuffer(CMWRenderBuffer* t_p_buf);

protected:
	int GetChannelIndex(int nIndex);
protected:
	HANDLE			m_hVideoThread;
	HANDLE			m_hExitVideoThread;

	HANDLE			m_hAudioThread;
	HANDLE			m_hExitAudioThread;

	HCHANNEL		m_hChannel;

	int				m_cx;
	int				m_cy;

	MWCAP_VIDEO_COLOR_FORMAT			m_colorFormat;
	MWCAP_VIDEO_QUANTIZATION_RANGE		m_quantRange;

	DWORD			m_dwFourcc;
	DWORD			m_dwFrameDuration;

	ICaptureCallback*	m_pCallback;
	DWORD				m_dwParam;

	double			m_dFramePerSecond;
	double			m_dSamplePerSecond;

	BOOL			m_bAudioValid;
	BOOL			m_bLPCM;
	BYTE			m_cBitsPerSample;
	DWORD			m_dwSampleRate;

	BOOL			m_bVideoValid;

	BOOL			m_bCaptureAudio;

	// adjust video frame
	LONGLONG		m_llVideoFrame;
	LONGLONG		m_llAudioSample;

	BOOL			m_bRunning;
	int				m_nClip;

	CMWRenderBuffer*	m_p_buf;
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;