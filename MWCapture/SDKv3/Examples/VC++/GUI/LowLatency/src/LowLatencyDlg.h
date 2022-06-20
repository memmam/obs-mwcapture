/************************************************************************************************/
// LowLatencyDlg.h : header file

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
#include "afxwin.h"

#include "VideoWnd.h"
#include <stdio.h>
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
#include <omp.h>
#include <queue>
#include <list>
using namespace std;

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080
#define CAPTURE_DURATION 166667
#define CAPTURE_COLOR MWFOURCC_RGBA // MWFOURCC_BGRA // MWFOURCC_YUY2

#define BIT_DEPTH 4
#define CAPTURE_PART_LINE 64

// CLowLatencyDlg dialog
class CLowLatencyDlg : public CDialogEx {
	// Construction
public:
	CLowLatencyDlg(CWnd *pParent = NULL); // standard constructor

	// Dialog Data
	enum { IDD = IDD_LOWLATENCY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

	// Implementation
protected:
	typedef enum { MODE_NORMAL, MODE_LOW_LATENCY } CAPTURE_MODE;

	typedef struct tagCAPTURE_DATA {
		HCHANNEL hChannel;
		HANDLE hThread;
		HANDLE hEvent;

		MWCAP_VIDEO_SIGNAL_STATUS status;

		DWORD dwCapFrameCount;
		DWORD dwHandleFrameCount;
		LONGLONG llCaptureDuration;
		LONGLONG llTotalDuration;
		LONGLONG llGpegHandleDuration;
		LONGLONG llTotalTime;
		int nSel;

		CVideoWnd wnd;
	} CAPTURE_DATA, *PCAPTURE_DATA;

	CAPTURE_DATA m_data[2];
	typedef struct tagCOMPRESS_DATA {
		BYTE *pBuffer;
		BOOL bStart;
		BOOL bCompleted;
		int nCyCompleted;
		LONGLONG llCapEnd;
	} COMPRESS_DATA, *PCOMPRESS_DATA;
	typedef struct tagJPEG_HANDLE_DATA {
		int nCount;
		BOOL bRun;
		HANDLE hThread;
		CRITICAL_SECTION cs;
	} JPEG_HANDLE_DATA, *PJPEG_HANDLE_DATA;

	HICON m_hIcon;

protected:
	static DWORD WINAPI ThreadProc(LPVOID pvParam)
	{
		CLowLatencyDlg *pThis = (CLowLatencyDlg *)pvParam;
		return pThis->ThreadProc(MODE_NORMAL);
	}

	static DWORD WINAPI ThreadProc1(LPVOID pvParam)
	{
		CLowLatencyDlg *pThis = (CLowLatencyDlg *)pvParam;
		return pThis->ThreadProc(MODE_LOW_LATENCY);
	}

	DWORD ThreadProc(CAPTURE_MODE mode);

	static DWORD WINAPI OnNormalJpegCompressThread(LPVOID pvParam)
	{
		CLowLatencyDlg *pThis = (CLowLatencyDlg *)pvParam;
		return pThis->OnNormalJpegCompressThreadProc(MODE_NORMAL);
	}
	DWORD OnNormalJpegCompressThreadProc(CAPTURE_MODE mode);

	static DWORD WINAPI OnLowlatencyJpegCompressThread(LPVOID pvParam)
	{
		CLowLatencyDlg *pThis = (CLowLatencyDlg *)pvParam;
		return pThis->OnLowlatencyJpegCompressThreadProc(
			MODE_LOW_LATENCY);
	}
	DWORD OnLowlatencyJpegCompressThreadProc(CAPTURE_MODE mode);

	BOOL OpenChannel(CAPTURE_MODE mode, int nIndex);
	void CloseChannel(CAPTURE_MODE mode);

	CString GetStatus(MWCAP_VIDEO_SIGNAL_STATE status)
	{
		CString strRet;
		switch (status) {
		case MWCAP_VIDEO_SIGNAL_NONE:
			strRet = _T("None");
			break;
		case MWCAP_VIDEO_SIGNAL_UNSUPPORTED:
			strRet = _T("Unsupported");
			break;
		case MWCAP_VIDEO_SIGNAL_LOCKING:
			strRet = _T("Locking");
			break;
		case MWCAP_VIDEO_SIGNAL_LOCKED:
			strRet = _T("Locked");
			break;
		}
		return strRet;
	}
	CString GetResolutions(int cx, int cy, DWORD dwFrameDuration)
	{
		CString strRet;
		strRet.Format(_T("%d x %d, %.02f FPS"), cx, cy,
			      (double)10000000 / dwFrameDuration);
		return strRet;
	}
	CString GetScanSize(int cx, int cy)
	{
		CString strRet;
		strRet.Format(_T("%d x %d Pixels"), cx, cy);
		return strRet;
	}
	CString GetOffset(int x, int y)
	{
		CString strRet;
		strRet.Format(_T("X:%d, Y:%d"), x, y);
		return strRet;
	}
	CString GetFormat(DWORD dwFrameDuration)
	{
		CString strRet;
		strRet.Format(_T("%d x %d, RGBA, %.02f fps"), CAPTURE_WIDTH,
			      CAPTURE_HEIGHT,
			      (double)10000000 / dwFrameDuration);
		return strRet;
	}
	CString GetLatency(DWORD dwFrameDuration)
	{
		CString strRet;
		strRet.Format(_T("%.02f ms"), (float)dwFrameDuration / 10000);
		return strRet;
	}
	CString GetFPS(float dwFrameDuration)
	{
		CString strRet;
		strRet.Format(_T("%.02f fps"),
			      (float)dwFrameDuration * 10000000);
		return strRet;
	}
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbDevice0;
	CComboBox m_cmbDevice1;
	CString m_strSignal0;
	CString m_strSignal1;
	CString m_strResolution0;
	CString m_strScanSize0;
	CString m_strOffset0;
	CString m_strFormat0;
	CString m_strCaptureLatency0;
	CString m_strTotalLatency0;
	CString m_strResolution1;
	CString m_strScanSize1;
	CString m_strOffset1;
	CString m_strFormat1;
	CString m_strCaptureLatency1;
	CString m_strTotalLatency1;
	//CString m_strCurrentFps0;
	//CString m_strCurrentFps1;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCombo0();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnMove(int x, int y);
	virtual BOOL PreTranslateMessage(MSG *pMsg);

protected:
	void InitJPEGfile(BYTE *pBuffer);

protected:
	JPEG_HANDLE_DATA m_DataJpeg[2];
	queue<COMPRESS_DATA> m_queNormal;
	COMPRESS_DATA m_DataNormal[5];
	queue<COMPRESS_DATA> m_queLowlatency;
	COMPRESS_DATA m_DataLowlatency[30];

public:
	CString m_strHandleLatency0;
	CString m_strHandleLatency1;
};
