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
#include "afxwin.h"

#include "VideoWnd.h"
#include "VideoSend.h"

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080
#define CAPTURE_DURATION 333334
#define CAPTURE_COLOR MWFOURCC_RGBA // MWFOURCC_BGRA // MWFOURCC_YUY2

// CNDISenderDlg 对话框
class CNDISenderDlg : public CDialogEx {
	// 构造
public:
	CNDISenderDlg(CWnd *pParent = NULL); // 标准构造函数

	// 对话框数据
	enum { IDD = IDD_NDISENDER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV 支持

protected:
	typedef struct tagCAPTURE_FORMAT {
		int cx;
		int cy;
		char *szFormat;
		DWORD fps;
		DWORD dwFourcc;

	} CAPTURE_FORMAT, *PCAPTURE_FORMAT;

	CAPTURE_FORMAT m_format[100];
	// 实现
protected:
	HICON m_hIcon;
	int m_nChannelIndex;
	int m_nSelectIndex;
	int m_nFormatSelectIndex;
	HCHANNEL m_hChannel;
	CVideoWnd m_videoWnd;
	CVideoSend m_videoSend;

	BOOL m_bShowTime;
	BOOL m_bUpdateChannel;
	CTime m_time;

protected:
	BOOL UpdateChannel();

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
	CString GetResolutions(int cx, int cy, DWORD dwFrameDuration,
			       BOOLEAN bInterlaced)
	{
		CString strRet;
		if (bInterlaced == FALSE)
			strRet.Format(_T("%d x %d, %.02f FPS"), cx, cy,
				      (double)10000000 / dwFrameDuration);
		else
			strRet.Format(_T("%d x %d, %.02f FPS"), cx, cy,
				      ((double)10000000 / dwFrameDuration) * 2);

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

	CString GetAspectRatio(int x, int y)
	{
		CString strRet;
		strRet.Format(_T("%d : %d"), x, y);
		return strRet;
	}

	CString GetColorFormat(MWCAP_VIDEO_COLOR_FORMAT colorFormat)
	{

		CString strRet;
		switch (colorFormat) {
		case MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN: {
			CAutoConvertString strName("unknown");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}

		case MWCAP_VIDEO_COLOR_FORMAT_RGB: {
			CAutoConvertString strName("RGB");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}
		case MWCAP_VIDEO_COLOR_FORMAT_YUV601: {
			CAutoConvertString strName("YUV601");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}
		case MWCAP_VIDEO_COLOR_FORMAT_YUV709: {
			CAutoConvertString strName("YUV709");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}
		case MWCAP_VIDEO_COLOR_FORMAT_YUV2020: {
			CAutoConvertString strName("YUV2020");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}
		case MWCAP_VIDEO_COLOR_FORMAT_YUV2020C: {
			CAutoConvertString strName("YUV2020C");
			strRet.Format(_T("%s"), (const TCHAR *)strName);
			break;
		}
		default: {
			CString strRet;
			strRet.Format(_T("-"));
			break;
		}
		}
		return strRet;
	}

	CString GetAudioFormat(BOOLEAN bLPCM, BYTE cBitsPerSample,
			       DWORD dwSampleRate)
	{
		if (bLPCM == 1) {
			CString strRet;
			strRet.Format(_T("%d Hz, %d Bits, LPCM"), dwSampleRate,
				      cBitsPerSample);
			return strRet;
		} else {
			CString strRet;
			strRet.Format(_T("-"));
			return strRet;
		}
	}

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CButton m_btnStop;
	CButton m_btnStart;
	CComboBox m_cmbDevice;
	CComboBox m_cmbCaptureFormat;
	CEdit m_edtSendName;

	CString m_strSignal;
	CString m_strResolution;
	CString m_strTotalSize;
	CString m_strOffset;
	CString m_strAspectRatio;
	CString m_strColorFormat;
	CString m_strAudioFormat;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCombo0();
	afx_msg void OnCbnSelchangeComboxFormat();
	afx_msg void OnMove(int x, int y);

	afx_msg void OnBnClickedBtnexit();
	afx_msg void OnBnClickedBtnstart();
	afx_msg void OnBnClickedBtnstop();
	afx_msg void OnStnClickedStaticFormat();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};
