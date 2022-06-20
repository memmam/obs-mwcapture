/************************************************************************************************/
// MultiAudioCaptureDlg.h : header file

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

#include "LibMWCapture\MWCapture.h"
#include "WaveFile.h"
#include "ChartCtrl\ChartCtrl.h"
#include "ChartCtrl\ChartLineSerie.h"
#include "afxwin.h"
#include "MWMultiDSRender\MWMultiDSRender.h"

#define PCM_FILE_NAME		"MultiAudioCapture"
#define PCM_FILE_SUFFIX		".wav"
#define ENCODED_FILE_NAME	"MultiAudioCapture"
#define ENCODED_FILE_SUFFIX	".raw"

#define VIEW_PERCENT 96
#define VIEW_SAMPLES_PER_FRAME (MWCAP_AUDIO_SAMPLES_PER_FRAME / VIEW_PERCENT)

#define UPDATE_LENGTH 40
#define VIEW_LENGTH	(UPDATE_LENGTH * 5)

#define MAX_BIT_DEPTH_IN_BYTE (sizeof(DWORD))

#define WM_REFRESH_LINESERIE_MSG WM_USER+100
#define WM_REFRESH_CHANNELINFO_MSG WM_USER+101

//#define ALWAYS_USE_16_BITS_PER_SAMPLE

// CMultiAudioCaptureDlg dialog
class CMultiAudioCaptureDlg : public CDialogEx
{
// Construction
public:
	CMultiAudioCaptureDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MULTIAUDIOCAPTURE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
	HCHANNEL						m_hChannel;
	HANDLE							m_hAudioEvent;
	
	MWCAP_AUDIO_SIGNAL_STATUS		m_audioSignalStatus;
	MWCAP_AUDIO_CAPTURE_FRAME		m_audioFrame;
	BOOL							m_bIsLPCM;
	char							m_szEncodedFmt[MAX_PATH];
	char							m_szEncodedFileFmt[MAX_PATH];
	int								m_nPresentChannel;
	int								m_nBitDepthInByte;

	BOOL							m_bAudioCapturing;
	HANDLE							m_hAudioCaptureThread;
	
	BYTE							m_byAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * MAX_BIT_DEPTH_IN_BYTE];
	BYTE							m_byPlaySamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * MAX_BIT_DEPTH_IN_BYTE];
	BYTE							m_byEncodedAudioSamples[MWCAP_AUDIO_SAMPLES_PER_FRAME * MWCAP_AUDIO_MAX_NUM_CHANNELS * MAX_BIT_DEPTH_IN_BYTE];
	
	BOOL							m_bRecording;
	BOOL							m_bStartRecord;
	CWaveFile						m_file;
	FILE*							m_pEncodedFile;
	LONGLONG						m_nFileSize;

	char							m_szLastRecordFile[MAX_PATH];
	
	// Used only when saving encoded audio 
	BYTE							m_byBurstHead[4]; // without F8 72 4E 1F sync tag
	int								m_cbBurstHead;
	BOOL							m_bGotSyncTag;
	WORD							m_nBurstSize;

	// View wave
	CComboBox						m_coChannelName;
	CChartCtrl						m_chart[MWCAP_AUDIO_MAX_NUM_CHANNELS];
	CChartLineSerie *				m_pLine[MWCAP_AUDIO_MAX_NUM_CHANNELS];

	int								m_nUpdatePos;
	double							m_szFrameX[UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME];
	double							m_szFrameY[MWCAP_AUDIO_MAX_NUM_CHANNELS][UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME];

	// Bottom status
	CHAR							m_szBottomStatus[MAX_PATH];

	CMutex							m_mutexChart;

	MWMultiDSRender					*m_p_multi_ds_render;
	bool							m_b_render;
	bool							m_b_dsound;
	bool							m_b_play_stream;
	bool							m_b_stream_play;
	int								m_n_playchannels;

	HANDLE							m_h_reset_device_event;

private:
	static DWORD WINAPI AudioCaptureThreadProc(LPVOID pvParam) {
		CMultiAudioCaptureDlg* pThis = (CMultiAudioCaptureDlg *)pvParam;
		return pThis->AudioCaptureThreadProcEx();
	}
	DWORD AudioCaptureThreadProcEx();

	static void device_callback(mw_device_role t_role,void *t_p_param){
		CMultiAudioCaptureDlg *t_p_this=(CMultiAudioCaptureDlg*)t_p_param;
		t_p_this->device_callback_impl(t_role);
	}

	void device_callback_impl(mw_device_role t_role);

	BOOL UpdateInfo();
	void InitChart();
	void UninitChart();
	void ViewFrame();
	void UpdateBottomStatus();
	void UpdateWaveView();
	void ParseEncodedAudioFrame();

	void set_audio_fmt(MWCAP_AUDIO_SIGNAL_STATUS t_status, mw_audio_format_t *t_p_fmt);

public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	BOOL m_bDisplay;
	afx_msg void OnBnClickedCheckWave();
	afx_msg LRESULT OnRefreshLineSerieHandle(WPARAM, LPARAM);
	afx_msg LRESULT OnRefreshChannelInfoHandle(WPARAM, LPARAM);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
	afx_msg void OnCbnSelchangeChannelCombo();
};

extern int g_nValidChannel[];
extern int g_nValidChannelCount;