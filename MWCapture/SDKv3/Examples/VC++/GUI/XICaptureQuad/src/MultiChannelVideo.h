/************************************************************************************************/
// MultiChannelVideo.h : interface of the CMultiChannelVideo class

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
#include "MWFOURCC.h"

class IVideoChannelCallback {
public:
	virtual void VideoCaptureCallback(int iChannel, LPCRECT lprcPosition,
					  const BYTE *pbyImage,
					  int cbImageStride) = NULL;
};

class CVideoChannel {
public:
	CVideoChannel()
	{
		m_hStream = NULL;
		m_pCallback = NULL;
		SetRectEmpty(&m_rcPosition);

		m_dCurrentFps = 0.0;
		m_llLastTime = 0;
		m_nLastFrameCount = 0;
	}

	virtual ~CVideoChannel() { Destroy(); }

public:
	BOOL Create(int iChannel, int iDevice, int iChannelInDevice,
		    DWORD dwColorFormat, int cx, int cy, int nFrameDuration,
		    IVideoChannelCallback *pCallback)
	{
		m_hStream = XIM_OpenVideoChannel(iDevice, iChannelInDevice);
		if (NULL == m_hStream)
			return FALSE;

		m_captureParams.cx = cx;
		m_captureParams.cy = cy;
		m_captureParams.dwDuration = nFrameDuration;
		m_captureParams.dwFourcc = dwColorFormat;
		m_captureParams.pFuncCallback = VideoCaptureCallback;
		m_captureParams.pvParam = this;
		m_captureParams.bBottomUp = TRUE;

		m_iChannel = iChannel;
		m_pCallback = pCallback;
		return TRUE;
	}

	void Destroy()
	{
		if (m_hStream) {
			XIM_StopVideoCapture(m_hStream);
			XIM_CloseVideoChannel(m_hStream);
			m_hStream = NULL;
		}
	}

	BOOL SetPosition(LPCRECT lprcPosition)
	{
		return CopyRect(&m_rcPosition, lprcPosition);
		;
	}

	BOOL Start()
	{
		m_dCurrentFps = 0.0;
		m_llLastTime = 0;
		m_nLastFrameCount = 0;
		return ::XIM_StartVideoCaptureByCallback(m_hStream,
							 &m_captureParams);
	}

	void Stop() { XIM_StopVideoCapture(m_hStream); }

	void GetPosition(LPRECT lprcPosition)
	{
		CopyRect(lprcPosition, &m_rcPosition);
	}

	int GetChannelIndex() { return m_iChannel; }

	void OnNewFrame()
	{
		m_nLastFrameCount++;

		LONGLONG llNow = GetTickCount();
		if (llNow - m_llLastTime >= 1000) {
			m_dCurrentFps =
				((double)m_nLastFrameCount) /
				(((double)(llNow - m_llLastTime)) / 1000);
			m_nLastFrameCount = 0;
			m_llLastTime = llNow;
		}
	}

	double GetCurrentFps() { return m_dCurrentFps; }

protected:
	static void VideoCaptureCallback(const BYTE *pbyImage, DWORD cbFrame,
					 int cbImageStride, void *pvParam,
					 UINT64 u64TimeStamp)
	{
		CVideoChannel *pThis = (CVideoChannel *)pvParam;
		pThis->m_pCallback->VideoCaptureCallback(pThis->m_iChannel,
							 &pThis->m_rcPosition,
							 pbyImage,
							 cbImageStride);
		pThis->OnNewFrame();
	}

protected:
	HANDLE m_hStream;
	RECT m_rcPosition;
	int m_iChannel;

	XIM_VIDEO_CAPTURE_PARAMS m_captureParams;

	IVideoChannelCallback *m_pCallback;

	double m_dCurrentFps;
	LONGLONG m_llLastTime;
	int m_nLastFrameCount;
};

class CMultiChannelVideo : public IVideoChannelCallback {
public:
	CMultiChannelVideo()
	{
		m_hRenderer = NULL;
		m_dwChannelMasks = 0;
		m_dwChannelDrawn = 0;
		InitializeCriticalSection(&m_cs);
	}

	virtual ~CMultiChannelVideo()
	{
		Destroy();
		DeleteCriticalSection(&m_cs);
	}

public:
	BOOL Create(HWND hWnd, int cHor, int cVer, DWORD dwColorFormat, int cx,
		    int cy, int nFrameDuration)
	{
		OSVERSIONINFO osvi;
		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osvi);

		m_hRenderer = XIM_CreateDDrawRenderer(
			NULL, hWnd,
			(dwColorFormat == MWFOURCC_BGRA) ? MWFOURCC_RGBA
							 : dwColorFormat,
			cHor * cx, cVer * cy, osvi.dwMajorVersion <= 5, TRUE);
		if (NULL == m_hRenderer)
			return FALSE;

		int cDeviceCount = XIM_GetDeviceCount();
		if (cDeviceCount <= 0) {
			Destroy();
			return FALSE;
		}

		m_dwChannelMasks = 0;
		int iDevice = 0;
		int iChannel = 0;
		XIM_DEVICE_INFO deviceInfo;

		if (!XIM_GetDeviceInfo(iDevice, &deviceInfo))
			return FALSE;

		for (int yStart = 0; yStart < cVer * cy &&
				     iDevice < cDeviceCount &&
				     m_arrChannels.GetCount() < cHor * cVer;) {
			for (int xStart = 0;
			     xStart < cHor * cx && iDevice < cDeviceCount &&
			     m_arrChannels.GetCount() < cHor * cVer;
			     ++iChannel) {

				if (iChannel >= (int)deviceInfo.dwVideoCount) {
					iDevice++;
					if (XIM_GetDeviceInfo(iDevice,
							      &deviceInfo)) {
						iChannel = -1;
					}
					continue;
				}

				CVideoChannel *pChannel = new CVideoChannel();
				if (NULL == pChannel)
					continue;

				if (!pChannel->Create(
					    GetChannelCount(), iDevice,
					    deviceInfo.videoInfo[iChannel]
						    .byChannelIndex,
					    dwColorFormat, cx, cy,
					    nFrameDuration, this)) {
					delete pChannel;
					continue;
				}

				m_dwChannelMasks |=
					(1 << m_arrChannels.GetCount());

				RECT rcChannel;
				rcChannel.left = xStart;
				rcChannel.top = yStart;
				rcChannel.right = xStart + cx;
				rcChannel.bottom = yStart + cy;

				pChannel->SetPosition(&rcChannel);

				xStart += cx;

				m_arrChannels.Add(pChannel);
			}
			yStart += cy;
		}

		if (m_arrChannels.GetCount() <= 0) {
			Destroy();
			return FALSE;
		}

		// Fill backbuffer with black
		XIM_VideoRendererClearImage(m_hRenderer);

		m_hWnd = hWnd;
		UpdateRendererPosition();

		StartCaptures();
		return TRUE;
	}

	void Destroy()
	{
		// Destroy video channels
		for (int i = 0; i < m_arrChannels.GetCount(); i++) {
			CVideoChannel *pChannel =
				(CVideoChannel *)m_arrChannels[i];
			delete pChannel;
		}
		m_arrChannels.RemoveAll();

		m_dwChannelMasks = 0;

		// Destroy renderer
		if (m_hRenderer) {
			XIM_DestroyDDrawRenderer(m_hRenderer);
			m_hRenderer = NULL;
		}
	}

	void Repaint() { XIM_VideoRendererRepaintRect(m_hRenderer, NULL); }

	void UpdateRendererPosition()
	{
		if (m_hRenderer) {
			RECT rcClient;
			GetClientRect(m_hWnd, &rcClient);
			XIM_SetDDrawRendererPosition(m_hRenderer, NULL,
						     &rcClient);
		}
	}

	int GetChannelCount() { return (int)m_arrChannels.GetCount(); }

	double GetCurrentFps(int nId)
	{
		if (nId >= GetChannelCount()) {
			return 0.0;
		}
		CVideoChannel *pChannel = (CVideoChannel *)m_arrChannels[nId];
		return pChannel->GetCurrentFps();
	}

protected:
	void VideoCaptureCallback(int iChannel, LPCRECT lprcPosition,
				  const BYTE *pbyImage, int cbImageStride)
	{
		int cxSrc = (lprcPosition->right - lprcPosition->left) >>
			    1 << 1;
		int cySrc = (lprcPosition->bottom - lprcPosition->top);
		if (cxSrc <= 0 || cySrc <= 0)
			return;

		EnterCriticalSection(&m_cs);

		XIM_DDrawRendererBeginBatchDraw(m_hRenderer);

		XIM_DDrawRendererBatchDrawRect(m_hRenderer, pbyImage, cxSrc,
					       cySrc, cbImageStride, FALSE,
					       lprcPosition->left,
					       lprcPosition->top);

		m_dwChannelDrawn |= (1 << iChannel);
		if (m_dwChannelDrawn == m_dwChannelMasks) {
			XIM_DDrawRendererEndBatchDraw(m_hRenderer, TRUE);
			m_dwChannelDrawn = 0;
		} else
			XIM_DDrawRendererEndBatchDraw(m_hRenderer, FALSE);

		LeaveCriticalSection(&m_cs);
	}

	void StopCaptures()
	{
		for (int i = 0; i < m_arrChannels.GetCount(); i++) {
			CVideoChannel *pChannel =
				(CVideoChannel *)m_arrChannels[i];
			pChannel->Stop();
		}
	}

	void StartCaptures()
	{
		for (int i = 0; i < m_arrChannels.GetCount(); i++) {
			CVideoChannel *pChannel =
				(CVideoChannel *)m_arrChannels[i];
			pChannel->Start();
		}
	}

protected:
	CRITICAL_SECTION m_cs;
	CPtrArray m_arrChannels;
	HWND m_hWnd;
	HANDLE m_hRenderer;
	int m_cx;
	int m_cy;
	DWORD m_dwChannelMasks;
	DWORD m_dwChannelDrawn;

	double m_dCurrentFps;
	LONGLONG m_llLastTime;
	int m_nLastFrameCount;
};
