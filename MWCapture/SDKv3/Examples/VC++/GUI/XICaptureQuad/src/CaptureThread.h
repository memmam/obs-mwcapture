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

// Reg Define
#define REG_ROOT					HKEY_CURRENT_USER
#define REG_NODE					_T("Software\\Magewell\\VMWCapture SDK V4.0\\XICaptureQuad")

#define	REG_CHANNEL_IDS				_T("channel_ids")
#define REG_LAYOUT_X				_T("layout_x")
#define REG_LAYOUT_Y				_T("layout_y")
#define REG_WIDTH					_T("width")
#define REG_HEIGHT					_T("height")
#define REG_FRAME_DURATION			_T("framerate")
#define REG_SYNC					_T("sync")

#define MAX_CHANNEL_COUNT	4

typedef struct tagCHANNEL_INDEX {
	int			nIndex;
	int			nBoard;
	int			nChannel;
} CHANNEL_INDEX, *PCHANNEL_INDEX;


class ICaptureCallback
{
public:
	virtual void OnVideoSignalChanged(int cx, int cy, DWORD dwParam) = NULL;
	virtual void OnCaptureCallback(const BYTE* pbyData, int cbStride, DWORD dwParam) = NULL;
};

inline BOOL MWCheckTime(LONGLONG llStdTime, LONGLONG llTime, DWORD dwFrameDuration, float fRange)
{
	LONGLONG llOffset = (LONGLONG)(dwFrameDuration * fRange);
	return (llTime >= (llStdTime - llOffset) && llTime <= (llStdTime + llOffset));
};

inline DWORD MWParseChannels(LPCTSTR lpChannels, CHANNEL_INDEX* pChannelItem) 
{
	DWORD dwReturn = 0;

	const TCHAR* pNext = lpChannels;
	for (int i = 0; i < MAX_CHANNEL_COUNT; ++i) {
		if (_tcslen(pNext) < 1)
			break;

		pChannelItem[i].nBoard = _ttoi(pNext);
		const TCHAR* pLine = _tcsstr(pNext, _T("-"));
		if (pLine == NULL)
			break;

		pLine++;
		pChannelItem[i].nChannel = _ttoi(pLine);

		dwReturn++;

		pNext = _tcsstr(pNext, _T(","));
		if (pNext == NULL)
			break;

		pNext++;
	}

	return dwReturn;
}

inline BOOL MWReadFromReg(CHANNEL_INDEX* pChannelItem, int* pnChannelCount, int* pnLayoutX, int* pnLayoutY, int* pnWidth, int* pnHeight, DWORD* pdwFrameDuration, BOOL* pbSync)
{
	HKEY key;
	LONG lResult;

	lResult = RegOpenKeyEx(REG_ROOT, REG_NODE, 0, KEY_READ, &key);
	if (ERROR_SUCCESS != lResult)
		return FALSE;

	TCHAR szChannels[256];
	DWORD dwSize = sizeof(szChannels);
	DWORD dwType = 0;

	lResult = RegQueryValueEx(key, REG_CHANNEL_IDS, 0, &dwType, (LPBYTE)szChannels, &dwSize);
	if(lResult == NO_ERROR) {
		if (pChannelItem != NULL) {
			*pnChannelCount = MWParseChannels(szChannels, pChannelItem);
		}
	}

	DWORD dwLayoutX = 0;
	lResult = RegQueryValueEx(key, REG_LAYOUT_X, 0, &dwType, (LPBYTE)&dwLayoutX, &dwSize);
	if(lResult == NO_ERROR) {
		*pnLayoutX = dwLayoutX;
	}
	
	DWORD dwLayoutY = 0;
	lResult = RegQueryValueEx(key, REG_LAYOUT_Y, 0, &dwType, (LPBYTE)&dwLayoutY, &dwSize);
	if(lResult == NO_ERROR) {
		*pnLayoutY = dwLayoutY;
	}
	
	DWORD dwWidth = 0;
	lResult = RegQueryValueEx(key, REG_WIDTH, 0, &dwType, (LPBYTE)&dwWidth, &dwSize);
	if(lResult == NO_ERROR && pnWidth != NULL) {
		*pnWidth = dwWidth;
	}
	
	DWORD dwHeight = 0;
	lResult = RegQueryValueEx(key, REG_HEIGHT, 0, &dwType, (LPBYTE)&dwHeight, &dwSize);
	if(lResult == NO_ERROR && pnHeight != NULL) {
		*pnHeight = dwHeight;
	}
	
	DWORD dwFrameDuration = 0;
	lResult = RegQueryValueEx(key, REG_FRAME_DURATION, 0, &dwType, (LPBYTE)&dwFrameDuration, &dwSize);
	if(lResult == NO_ERROR && dwFrameDuration > 0 && pdwFrameDuration != NULL) {
		*pdwFrameDuration = dwFrameDuration;
	}

	DWORD dwSync = 0;
	lResult = RegQueryValueEx(key, REG_SYNC, 0, &dwType, (LPBYTE)&dwSync, &dwSize);
	if(lResult == NO_ERROR && pbSync != NULL) {
		*pbSync = dwSync;
	}

	RegCloseKey(key);
	return TRUE;
}

inline BOOL MWWriteReg(LPCTSTR lpChannels, int nLayoutX, int nLayoutY, int nWidth, int nHeight, DWORD dwFrameDuration, BOOL bSync) {
	LSTATUS lStatus;
	HKEY key;
	lStatus = RegCreateKey(REG_ROOT, REG_NODE, &key);
	if (ERROR_SUCCESS != lStatus) {
		return FALSE;
	}

	do {
		lStatus = RegSetValueEx(key, REG_CHANNEL_IDS, 0, REG_SZ, (LPBYTE)lpChannels, ((DWORD)_tcslen(lpChannels) + 1) * sizeof(TCHAR));
		if (ERROR_SUCCESS != lStatus)
			break;

		DWORD dwValue = nLayoutX;
		lStatus = RegSetValueEx(key, REG_LAYOUT_X, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}

		dwValue = nLayoutY;
		lStatus = RegSetValueEx(key, REG_LAYOUT_Y, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}

		dwValue = nWidth;
		lStatus = RegSetValueEx(key, REG_WIDTH, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}

		dwValue = nHeight;
		lStatus = RegSetValueEx(key, REG_HEIGHT, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}

		dwValue = dwFrameDuration;
		lStatus = RegSetValueEx(key, REG_FRAME_DURATION, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}

		dwValue = bSync;
		lStatus = RegSetValueEx(key, REG_SYNC, 0, REG_DWORD, (LPBYTE)&dwValue, sizeof(DWORD));
		if (ERROR_SUCCESS != lStatus) {
			break;
		}
	} while (FALSE);

	RegCloseKey(key);

	return (ERROR_SUCCESS == lStatus) ? TRUE : FALSE;
}


class CCaptureThread
{
public:
	CCaptureThread();
	virtual ~CCaptureThread();

public:
	double GetFPS() {
		return m_dCurrentFps;
	}
	int GetValidChannel() {
		return m_nValidChannel;
	}

	BOOL Create(const CHANNEL_INDEX* pItems, int nNumChannel, int nLayoutX, int nLayoutY, int cx, int cy, DWORD dwFourcc, DWORD dwFrameDuration, BOOL bSyncMode, ICaptureCallback* pCallback, DWORD dwParam = 0);
	void Destroy();

	static DWORD WINAPI SignalThreadProc(LPVOID pvParam) {
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		return pThis->SignalThreadProc();
	}
	DWORD SignalThreadProc();

	static DWORD WINAPI TimerThreadProc(LPVOID pvParam) {
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		return pThis->TimerThreadProc();
	}
	DWORD TimerThreadProc();

	static DWORD WINAPI VideoThreadProc(LPVOID pvParam) {
		CCaptureThread* pThis = (CCaptureThread *)pvParam;
		return pThis->VideoThreadProc();
	}
	DWORD VideoThreadProc();
	
protected:
	typedef struct tagCHANNEL_ITEM {
		// for Create 
		int			nIndex;
		int			nBoard;
		int			nChannel;

		HCHANNEL	hChannel;
		HANDLE		hCaptureEvent;
		HANDLE		hNotifyEvent;
		HNOTIFY		hNotify;

		BYTE		byCapIndex;
		BYTE		byCurIndex;

		RECT		rcPos;
	} CHANNEL_ITEM, *PCHANNEL_ITEM;

	HANDLE				m_hTimerThread;
	BOOL				m_bExitTimerThread;
	BOOL				m_bGetFirst;
	LONGLONG			m_llTimeGetFirst;
	HANDLE				m_hSignalThread;
	HANDLE				m_hExitSignalThread;

	HANDLE				m_hVideoThread;
	HANDLE				m_hExitVideoThread;

	int					m_nNumChannel;
	CHANNEL_ITEM		m_arrChannel[MAX_CHANNEL_COUNT];

	CMWLock				m_lock;
	CMWLock				m_lockGetFirst;

	DWORD				m_dwCapMask;
	DWORD				m_dwCurMask;
	DWORD				m_dwStdMask;

	int					m_nLayoutX;
	int					m_nLayoutY;

	int					m_cx;
	int					m_cy;

	DWORD				m_dwFourcc;
	DWORD				m_dwFrameDuration;

	BOOL				m_bSyncMode;

	ICaptureCallback*	m_pCallback;
	DWORD				m_dwParam;

	LPBYTE				m_pbBuffer;
	int					m_cbBuffer;

	int					m_nValidChannel;
	double				m_dCurrentFps;
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;