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

#ifndef _CAPTURETHREAD_H
#define _CAPTURETHREAD_H

#include <Windows.h>
#include "StringUtils.h"
#include "LibMWCapture\MWCapture.h"
#include <iostream>
using namespace std;

#define CAPTURE_WIDTH		1920 
#define CAPTURE_HEIGHT		1080
#define CAPTURE_DURATION	333333//166667
#define CAPTURE_COLOR		MWFOURCC_NV12 // MWFOURCC_BGR24 // MWFOURCC_YUY2

#define	CAPTURE_PART_LINE	256


class ICaptureCallback
{
public:
	virtual BYTE* Lock() = 0;
	virtual BOOL UnLock() = 0;
};


class CCaptureThread
{
public:
	CCaptureThread();
	~CCaptureThread();

	int GetChannelCount();
	DWORD GetSignalState(){ return m_signalStatus; }
	DWORD GetFrameRate() { return 1000*10000/m_dwFrameDuration; }
	DWORD GetCaptureEndStatus();
	BOOL Create(int boardIndex,int channelIndex, ICaptureCallback* pCallback);
	void Destroy();

protected:
	static DWORD WINAPI ThreadProc(LPVOID pvParam){
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		return pThis->ThreadProcEx();
	};
	DWORD ThreadProcEx();

private:
	ICaptureCallback* m_pCallback;
	HCHANNEL m_hChannel;
	HANDLE   m_hThread;
	HANDLE	 m_hEvent;

	//
	int				m_cx;
	int				m_cy;
	DWORD			m_dwFOURCC;
	DWORD			m_dwFrameDuration;
	DWORD			m_signalStatus;

	//Status
	MWCAP_VIDEO_SIGNAL_STATUS	m_status;
	DWORD			m_dwMissingFrameNumber;
	int				m_nLastFrameIndex;
};

#endif