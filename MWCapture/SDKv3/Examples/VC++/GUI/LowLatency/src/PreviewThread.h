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

class IPreviewCallback {
public:
	virtual BOOL OnVideoCallback(const BYTE *lpData, DWORD cbStride) = NULL;
};

class CPreviewThread {
public:
	CPreviewThread();
	virtual ~CPreviewThread();

public:
	BOOL Create(int nIndex, int cx, int cy, DWORD dwFrameDuration,
		    DWORD dwFOURCC, IPreviewCallback *pCallback);
	void Destroy();

	static DWORD WINAPI ThreadProc(LPVOID pvParam)
	{
		CPreviewThread *pThis = (CPreviewThread *)pvParam;
		return pThis->ThreadProc();
	}
	DWORD ThreadProc();

protected:
	HCHANNEL m_hChannel;

	HANDLE m_hThread;
	HANDLE m_hEvent;

	BOOL m_bOnSignal;
	BOOL m_bLowLatency;

	int m_cx;
	int m_cy;

	DWORD m_dwFrameDuration;
	DWORD m_dwFOURCC;

	MWCAP_VIDEO_SIGNAL_STATUS m_status;

	IPreviewCallback *m_pCallback;
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;