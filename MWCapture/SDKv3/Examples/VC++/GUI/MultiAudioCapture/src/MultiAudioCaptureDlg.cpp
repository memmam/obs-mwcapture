/************************************************************************************************/
// MultiAudioCaptureDlg.cpp : implementation file

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
#include "stdafx.h"
#include "MultiAudioCapture.h"
#include "MultiAudioCaptureDlg.h"
#include "afxdialogex.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define RENDER_TIMER_ID 1
#define UPDATE_FILE_SIZE_TIMER_ID 2

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG *pMsg);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CMultiAudioCaptureDlg dialog

CMultiAudioCaptureDlg::CMultiAudioCaptureDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CMultiAudioCaptureDlg::IDD, pParent), m_bDisplay(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMultiAudioCaptureDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	for (int i = 0; i < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++i)
		DDX_Control(pDX, IDC_CHARTCTRL_WAVE + i, m_chart[i]);
	DDX_Check(pDX, IDC_CHECK_WAVE, m_bDisplay);
	DDX_Control(pDX, IDC_CHANNEL_COMBO, m_coChannelName);
}

BEGIN_MESSAGE_MAP(CMultiAudioCaptureDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_BUTTON_RECORD,
	      &CMultiAudioCaptureDlg::OnBnClickedButtonRecord)
ON_BN_CLICKED(IDC_BUTTON_PLAY, &CMultiAudioCaptureDlg::OnBnClickedButtonPlay)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_CHECK_WAVE, &CMultiAudioCaptureDlg::OnBnClickedCheckWave)
ON_MESSAGE(WM_REFRESH_LINESERIE_MSG,
	   &CMultiAudioCaptureDlg::OnRefreshLineSerieHandle)
ON_MESSAGE(WM_REFRESH_CHANNELINFO_MSG,
	   &CMultiAudioCaptureDlg::OnRefreshChannelInfoHandle)
ON_CBN_SELCHANGE(IDC_CHANNEL_COMBO,
		 &CMultiAudioCaptureDlg::OnCbnSelchangeChannelCombo)
END_MESSAGE_MAP()

// CMultiAudioCaptureDlg message handlers

BOOL CMultiAudioCaptureDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL) {
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty()) {
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX,
					     strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);  // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// TODO: Add extra initialization here
	m_bAudioCapturing = FALSE;
	m_hAudioCaptureThread = NULL;
	m_hChannel = NULL;
	m_bRecording = FALSE;
	m_bStartRecord = FALSE;
	m_pEncodedFile = NULL;
	m_hAudioEvent = NULL;

	memset(m_szEncodedFmt, 0, MAX_PATH);
	memset(m_szEncodedFileFmt, 0, MAX_PATH);
	memset(m_szLastRecordFile, 0, MAX_PATH);
	memset(m_szBottomStatus, 0, MAX_PATH);
	memset(m_byAudioSamples, 0, sizeof(m_byAudioSamples));
	memset(m_byPlaySamples, 0, sizeof(m_byPlaySamples));
	memset(m_byEncodedAudioSamples, 0, sizeof(m_byEncodedAudioSamples));

	GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(FALSE);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_BOTTOM_STATUS,
			m_szBottomStatus);

	InitChart();

	m_b_dsound = false;
	m_b_play_stream = false;
	m_b_stream_play = false;
	m_b_render = false;
	m_p_multi_ds_render = NULL;
	m_p_multi_ds_render = new MWMultiDSRender();
	m_h_reset_device_event = NULL;
	if (m_p_multi_ds_render != NULL) {
		m_b_render = true;
		mw_multi_ds_status_e t_mw_ret =
			m_p_multi_ds_render->initilize_dsound();
		if (t_mw_ret == MW_AUDIO_NO_ERROR) {
			m_b_dsound = true;
			m_h_reset_device_event =
				CreateEvent(NULL, FALSE, FALSE, NULL);
			m_p_multi_ds_render->register_default_device_changed(
				device_callback, this);
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Device Ready\0");
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		} else {
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Device Error\0");
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		}
	}

	CHAR szChannelName[MAX_PATH];

	USES_CONVERSION;
	for (int i = 0; i < g_nValidChannelCount; i++) {
		if (m_hChannel) {
			MWCloseChannel(m_hChannel);
			m_hChannel = NULL;
		}

		WCHAR szDevicePath[MAX_PATH];
		if (MW_SUCCEEDED !=
		    ::MWGetDevicePath(g_nValidChannel[i], szDevicePath)) {
			continue;
		}

		m_hChannel = ::MWOpenChannelByPath(szDevicePath);
		if (m_hChannel == NULL) {
			continue;
		}

		DWORD dwInputCount = 0;
		if (MW_SUCCEEDED !=
		    MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount))
			continue;

		if (dwInputCount == 0) {
			continue;
		}

		MWCAP_CHANNEL_INFO infoChannel = {0};
		if (MW_SUCCEEDED !=
		    ::MWGetChannelInfo(m_hChannel, &infoChannel)) {
			MessageBox(_T("Get channel info fail !"),
				   _T("MultiAudioCapture"));
			MessageBox(
				_T("Audio signal is changed and cannot get current aduio signal status. Exit ! Please restart !"),
				_T("MultiAudioCapture"));
			return FALSE;
		}

		sprintf_s(szChannelName, "%s %x:%x\0",
			  infoChannel.szProductName, infoChannel.byBoardIndex,
			  infoChannel.byChannelIndex);
		m_coChannelName.InsertString(i, A2W(szChannelName));

		if (m_hChannel) {
			MWCloseChannel(m_hChannel);
			m_hChannel = NULL;
		}
	}

	m_coChannelName.SetCurSel(0);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_AUDIO_DEVICE, szChannelName);

	WCHAR szDevicePath[MAX_PATH];
	::MWGetDevicePath(g_nValidChannel[0], szDevicePath);

	m_hChannel = ::MWOpenChannelByPath(szDevicePath);

	DWORD dwInputCount = 0;
	MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount);

	UpdateInfo();

	m_hAudioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (MW_SUCCEEDED != MWStartAudioCapture(m_hChannel)) {
		MessageBox(_T("Start capture audio fail !"),
			   _T("MultiAudioCapture"));
		return 0;
	}

	m_bAudioCapturing = TRUE;
	m_hAudioCaptureThread = CreateThread(NULL, 0, AudioCaptureThreadProc,
					     (LPVOID)this, 0, NULL);
	if (m_hAudioCaptureThread == NULL) {
		m_bAudioCapturing = FALSE;
		return 0;
	}

	return FALSE; // return TRUE  unless you set the focus to a control
}

void CMultiAudioCaptureDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMultiAudioCaptureDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND,
			    reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMultiAudioCaptureDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

WCHAR *charToWchar(char *s)
{
	int w_nlen = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	WCHAR *ret;
	ret = (WCHAR *)malloc(sizeof(WCHAR) * w_nlen);
	memset(ret, 0, sizeof(ret));
	MultiByteToWideChar(CP_ACP, 0, s, -1, ret, w_nlen);
	return ret;
}

void CMultiAudioCaptureDlg::device_callback_impl(mw_device_role t_role)
{
	if (m_h_reset_device_event != NULL) {
		if (t_role == MW_MM_MULTIMEDIA)
			SetEvent(m_h_reset_device_event);
	}
}

BOOL CMultiAudioCaptureDlg::UpdateInfo()
{
	if (m_hChannel == NULL)
		return FALSE;

	MWCAP_CHANNEL_INFO infoChannel = {0};
	if (MW_SUCCEEDED != ::MWGetChannelInfo(m_hChannel, &infoChannel)) {
		MessageBox(_T("Get channel info fail !"),
			   _T("MultiAudioCapture"));
		MessageBox(
			_T("Audio signal is changed and cannot get current aduio signal status. Exit ! Please restart !"),
			_T("MultiAudioCapture"));
		return FALSE;
	}

	CHAR szChannelName[MAX_PATH];
	sprintf_s(szChannelName, "%s %x:%x\0", infoChannel.szProductName,
		  infoChannel.byBoardIndex, infoChannel.byChannelIndex);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_AUDIO_DEVICE, szChannelName);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_SERIAL_NUMBER,
			infoChannel.szBoardSerialNo);

	CHAR szVersion[MAX_PATH];
	sprintf_s(szVersion, "%d.%d.%d", infoChannel.dwDriverVersion >> 24,
		  (infoChannel.dwDriverVersion >> 16) & 0x00ff,
		  infoChannel.dwDriverVersion & 0x00ffff);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_DRIVER, szVersion);

	CHAR szFirmware[MAX_PATH];
	sprintf_s(szFirmware, "%d.%d", infoChannel.dwFirmwareVersion >> 16,
		  infoChannel.dwFirmwareVersion & 0x00ffff);
	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_FIRMWARE, szFirmware);

	DWORD dwSource = 0;
	if (MW_SUCCEEDED != ::MWGetAudioInputSource(m_hChannel, &dwSource)) {
		MessageBox(_T("Get audio input source fail !"),
			   _T("MultiAudioCapture"));
		MessageBox(
			_T("Audio signal is changed and cannot get current aduio signal status. Exit ! Please restart !"),
			_T("MultiAudioCapture"));
		return FALSE;
	}

	//MWCAP_AUDIO_INPUT_TYPE_NONE					= 0x00,
	//MWCAP_AUDIO_INPUT_TYPE_HDMI					= 0x01,
	//MWCAP_AUDIO_INPUT_TYPE_SDI					= 0x02,
	//MWCAP_AUDIO_INPUT_TYPE_LINE_IN				= 0x04,
	//MWCAP_AUDIO_INPUT_TYPE_MIC_IN					= 0x08

	switch (dwSource >> 8) {
	case MWCAP_AUDIO_INPUT_TYPE_HDMI:
		SetDlgItemText(IDC_STATIC_AUDIO_INPUT, _T("HDMI"));
		break;
	case MWCAP_AUDIO_INPUT_TYPE_SDI:
		SetDlgItemText(IDC_STATIC_AUDIO_INPUT, _T("SDI"));
		break;
	case MWCAP_AUDIO_INPUT_TYPE_LINE_IN:
		SetDlgItemText(IDC_STATIC_AUDIO_INPUT, _T("Line In"));
		break;
	case MWCAP_AUDIO_INPUT_TYPE_MIC_IN:
		SetDlgItemText(IDC_STATIC_AUDIO_INPUT, _T("MIC In"));
		break;
	default:
		SetDlgItemText(IDC_STATIC_AUDIO_INPUT, _T("NONE"));
		break;
	}

	if (MW_SUCCEEDED !=
	    MWGetAudioSignalStatus(m_hChannel, &m_audioSignalStatus)) {
		MessageBox(_T("Get audio signal status fail !"),
			   _T("MultiAudioCapture"));
		MessageBox(
			_T("Audio signal is changed and cannot get current aduio signal status. Exit ! Please restart !"),
			_T("MultiAudioCapture"));
		return FALSE;
	}

	if (m_audioSignalStatus.bLPCM) {
		m_bIsLPCM = TRUE;
	} else {
		m_bIsLPCM = FALSE;
	}

	if (m_audioSignalStatus.wChannelValid == 0) {
		SetDlgItemText(IDC_STATIC_AUDIO_SAMPLERATE, _T("None"));
	} else {
		CString strTitle;
		strTitle.Format(_T("%d Hz, %d bits, %s"),
				m_audioSignalStatus.dwSampleRate,
				m_audioSignalStatus.cBitsPerSample,
				m_bIsLPCM ? _T("LPCM") : _T("Encoded"));
		if (!m_bIsLPCM) {
			if (strlen(m_szEncodedFmt) > 0) {
				WCHAR *t_str = charToWchar(m_szEncodedFmt);
				strTitle.Format(
					_T("%d Hz, %d bits, %s"),
					m_audioSignalStatus.dwSampleRate,
					m_audioSignalStatus.cBitsPerSample,
					t_str);
				free(t_str);
			}
		}
		SetDlgItemText(IDC_STATIC_AUDIO_SAMPLERATE, strTitle);
		//SetDlgItemInt(IDC_STATIC_AUDIO_BITS_PER_SAMPLE, m_audioSignalStatus.cBitsPerSample);
	}

#ifdef ALWAYS_USE_16_BITS_PER_SAMPLE
	m_nBitDepthInByte = 2;
#else
	m_nBitDepthInByte = m_audioSignalStatus.cBitsPerSample / 8;
#endif

	for (int i = 0; i < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++i) {
		m_chart[i]
			.GetAxis(CChartCtrl::LeftAxis)
			->SetMinMax(-(0x01 << (m_nBitDepthInByte * 8 - 1)),
				    (0x01 << (m_nBitDepthInByte * 8 - 1)) - 1);
	}

	m_nPresentChannel = 0;
	for (int i = 0; i < 4; ++i) {
		BOOL bPresent =
			(m_audioSignalStatus.wChannelValid & (0x01 << i))
				? TRUE
				: FALSE;

		m_nPresentChannel += bPresent ? 2 : 0;

		SetDlgItemText(IDC_STATIC_CHANNEL12 + i,
			       bPresent ? _T("Present") : _T("Not Present"));
	}

	CString strData;
	for (int i = 0; i < 24; ++i) {
		strData.AppendFormat(
			_T("%02X "),
			m_audioSignalStatus.channelStatus.abyData[i]);
	}
	SetDlgItemText(IDC_STATIC_CHANNEL_STATUS_DATA, strData);

	return TRUE;
}

void CMultiAudioCaptureDlg::set_audio_fmt(MWCAP_AUDIO_SIGNAL_STATUS t_status,
					  mw_audio_format_t *t_p_fmt)
{
	t_p_fmt->m_n_bitspersample = t_status.cBitsPerSample;
	switch (t_status.cBitsPerSample) {
	case 8:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_08;
		break;
	case 16:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_16;
		break;
	case 20:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_20;
		break;
	case 24:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_24;
		break;
	case 32:
		t_p_fmt->m_audio_type = MW_AUDIO_FORMAT_PCM_32;
		break;
	}
	int t_n_channel_valid = 0;
	WORD t_w_channel = t_status.wChannelValid;
	for (int k = 0; k < 4; k++) {
		if (t_w_channel >> k != 0)
			t_n_channel_valid += 2;
	}
	t_p_fmt->m_n_channels = t_n_channel_valid;
	switch (t_n_channel_valid) {
	case 2:
		t_p_fmt->m_channel_type = MW_CHANNELS_2_0;
		break;
	case 4:
		t_p_fmt->m_channel_type = MW_CHANNELS_4_0;
		break;
	case 6:
		t_p_fmt->m_channel_type = MW_CHANNELS_6_0;
		break;
	case 8:
		t_p_fmt->m_channel_type = MW_CHANNELS_7_1;
		break;
	}

	t_p_fmt->m_n_sample_rate = t_status.dwSampleRate;
	t_p_fmt->m_n_timeuint = 1;
}

DWORD CMultiAudioCaptureDlg::AudioCaptureThreadProcEx()
{
	SetPriorityClass(m_hAudioCaptureThread, REALTIME_PRIORITY_CLASS);
	mw_multi_ds_status_e t_mw_ret = MW_AUDIO_NO_ERROR;

	HNOTIFY hAudioNotify =
		MWRegisterNotify(m_hChannel, m_hAudioEvent,
				 MWCAP_NOTIFY_AUDIO_INPUT_SOURCE_CHANGE |
					 MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE |
					 MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED);

	ULONGLONG ullStatusBits = 0LL;

	MWCAP_AUDIO_SIGNAL_STATUS audioSignalStatus;
	MWCAP_AUDIO_SIGNAL_STATUS t_audioSignalStatus;
	MWGetAudioSignalStatus(m_hChannel, &audioSignalStatus);

	bool t_b_caps = false;
	int t_n_channels = 0;
	if (m_b_render && m_b_dsound) {
		t_mw_ret = m_p_multi_ds_render->get_current_device_caps(
			&t_n_channels);
		if (t_mw_ret != MW_AUDIO_NO_ERROR) {
			t_b_caps = false;
		} else {
			t_b_caps = true;
		}
	}
	mw_audio_format_t t_format;
	set_audio_fmt(audioSignalStatus, &t_format);
	if (t_format.m_n_channels > t_n_channels) {
		m_n_playchannels = (t_n_channels - (t_n_channels % 2));
	} else {
		m_n_playchannels = t_format.m_n_channels;
	}
	t_format.m_n_channels = m_n_playchannels;
	t_mw_ret = MW_AUDIO_NO_ERROR;
	if (m_b_render && m_b_dsound && t_b_caps) {
		t_mw_ret = m_p_multi_ds_render->create_stream(&t_format);
		if (t_mw_ret == MW_AUDIO_NO_ERROR) {
			m_b_play_stream = true;
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Playing %d channels\0",
				  m_n_playchannels);
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		} else {
			m_b_play_stream = false;
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Playing Error\0");
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		}
	}

	if (m_b_render && m_b_dsound && m_b_play_stream) {
		t_mw_ret = m_p_multi_ds_render->run_stream();
		if (t_mw_ret == MW_AUDIO_NO_ERROR) {
			m_b_stream_play = true;
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Playing %d channels\0",
				  m_n_playchannels);
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		} else {
			m_b_stream_play = false;
			CHAR sz_palystatus[MAX_PATH];
			sprintf_s(sz_palystatus, "Playing Error\0");
			SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_PLAY_STATE,
					sz_palystatus);
		}
	}

	m_bGotSyncTag = FALSE;
	m_nBurstSize = -1;
	m_cbBurstHead = 0;

	HANDLE array_event[] = {m_h_reset_device_event, m_hAudioEvent};
	while (m_bAudioCapturing) {
		if (m_h_reset_device_event == NULL) {
			WaitForSingleObject(m_hAudioEvent, INFINITE);
		} else {
			DWORD t_ret = WaitForMultipleObjects(2, array_event,
							     FALSE, INFINITE);
			if (t_ret == WAIT_OBJECT_0) {
				MWGetAudioSignalStatus(m_hChannel,
						       &audioSignalStatus);

				if (m_b_render && m_b_dsound &&
				    m_b_play_stream && m_b_stream_play) {
					m_p_multi_ds_render->stop_stream();
					m_b_stream_play = false;
					m_p_multi_ds_render->destory_stream();
					m_b_play_stream = false;
					CHAR sz_palystatus[MAX_PATH];
					sprintf_s(sz_palystatus,
						  "Reset Device\0");
					SetDlgItemTextA(GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
				}

				bool t_b_caps = false;
				int t_n_channels = 0;
				if (m_b_render && m_b_dsound) {
					t_mw_ret =
						m_p_multi_ds_render
							->get_current_device_caps(
								&t_n_channels);
					if (t_mw_ret != MW_AUDIO_NO_ERROR) {
						t_b_caps = false;
					} else {
						t_b_caps = true;
					}
				}
				mw_audio_format_t t_format;
				set_audio_fmt(audioSignalStatus, &t_format);
				if (t_format.m_n_channels > t_n_channels) {
					m_n_playchannels = (t_n_channels -
							    (t_n_channels % 2));
				} else {
					m_n_playchannels =
						t_format.m_n_channels;
				}
				t_format.m_n_channels = m_n_playchannels;
				mw_multi_ds_status_e t_mw_ret =
					MW_AUDIO_NO_ERROR;
				if (m_b_render && m_b_dsound && t_b_caps) {
					t_mw_ret = m_p_multi_ds_render
							   ->create_stream(
								   &t_format);
					if (t_mw_ret == MW_AUDIO_NO_ERROR) {
						m_b_play_stream = true;
						CHAR sz_palystatus[MAX_PATH];
						sprintf_s(
							sz_palystatus,
							"Reset Device SUCCEED\0",
							m_n_playchannels);
						SetDlgItemTextA(
							GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
					} else {
						m_b_play_stream = false;
						CHAR sz_palystatus[MAX_PATH];
						sprintf_s(
							sz_palystatus,
							"Reset Device FAILED\0",
							m_n_playchannels);
						SetDlgItemTextA(
							GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
					}
				}

				if (m_b_render && m_b_dsound &&
				    m_b_play_stream) {
					t_mw_ret = m_p_multi_ds_render
							   ->run_stream();
					if (t_mw_ret == MW_AUDIO_NO_ERROR) {
						m_b_stream_play = true;
						CHAR sz_palystatus[MAX_PATH];
						sprintf_s(
							sz_palystatus,
							"Playing %d channels\0",
							t_format.m_n_channels);
						SetDlgItemTextA(
							GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
					} else {
						m_b_stream_play = false;
					}
				}

				m_nPresentChannel = 0;
				for (int i = 0; i < 4; ++i) {
					BOOL bPresent =
						(audioSignalStatus.wChannelValid &
						 (0x01 << i))
							? TRUE
							: FALSE;
					m_nPresentChannel += bPresent ? 2 : 0;
				}
				m_nBitDepthInByte =
					t_format.m_n_bitspersample / 8;
			}
		}

		if (MW_SUCCEEDED !=
		    MWGetNotifyStatus(m_hChannel, hAudioNotify, &ullStatusBits))
			continue;

		if (ullStatusBits & (MWCAP_NOTIFY_AUDIO_SIGNAL_CHANGE |
				     MWCAP_NOTIFY_AUDIO_INPUT_SOURCE_CHANGE)) {
			MWGetAudioSignalStatus(m_hChannel, &audioSignalStatus);
			OutputDebugString(_T("Audio Signal Changed\n"));

			if (m_b_render && m_b_dsound && m_b_play_stream &&
			    m_b_stream_play) {
				m_p_multi_ds_render->stop_stream();
				m_b_stream_play = false;
				m_p_multi_ds_render->destory_stream();
				m_b_play_stream = false;
				CHAR sz_palystatus[MAX_PATH];
				sprintf_s(sz_palystatus, "Reset Device\0");
				SetDlgItemTextA(GetSafeHwnd(),
						IDC_STATIC_PLAY_STATE,
						sz_palystatus);
			}

			if (m_bRecording) {
				OnBnClickedButtonRecord();
			}

			MWCAP_CHANNEL_INFO infoChannel = {0};
			DWORD dwSource = 0;
			if (m_hChannel != NULL &&
			    (MW_SUCCEEDED != ::MWGetChannelInfo(m_hChannel,
								&infoChannel) ||
			     MW_SUCCEEDED != ::MWGetAudioInputSource(
						     m_hChannel, &dwSource) ||
			     MW_SUCCEEDED !=
				     MWGetAudioSignalStatus(
					     m_hChannel, &audioSignalStatus))) {
				MessageBox(
					_T("Audio signal is changed and cannot get current aduio signal status. Exit ! Please restart !"),
					_T("MultiAudioCapture"));
				break;
			}

			bool t_b_caps = false;
			int t_n_channels = 0;
			if (m_b_render && m_b_dsound) {
				t_mw_ret = m_p_multi_ds_render
						   ->get_current_device_caps(
							   &t_n_channels);
				if (t_mw_ret != MW_AUDIO_NO_ERROR) {
					t_b_caps = false;
				} else {
					t_b_caps = true;
				}
			}
			mw_audio_format_t t_format;
			set_audio_fmt(audioSignalStatus, &t_format);
			if (t_format.m_n_channels > t_n_channels) {
				m_n_playchannels =
					(t_n_channels - (t_n_channels % 2));
			} else {
				m_n_playchannels = t_format.m_n_channels;
			}
			t_format.m_n_channels = m_n_playchannels;
			mw_multi_ds_status_e t_mw_ret = MW_AUDIO_NO_ERROR;
			if (m_b_render && m_b_dsound && t_b_caps) {
				t_mw_ret = m_p_multi_ds_render->create_stream(
					&t_format);
				if (t_mw_ret == MW_AUDIO_NO_ERROR) {
					m_b_play_stream = true;
					CHAR sz_palystatus[MAX_PATH];
					sprintf_s(sz_palystatus,
						  "Reset Device SUCCEED\0");
					SetDlgItemTextA(GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
				} else {
					m_b_play_stream = false;
					CHAR sz_palystatus[MAX_PATH];
					sprintf_s(sz_palystatus,
						  "Reset Device FAILED\0");
					SetDlgItemTextA(GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
				}
			}

			if (m_b_render && m_b_dsound && m_b_play_stream) {
				t_mw_ret = m_p_multi_ds_render->run_stream();
				if (t_mw_ret == MW_AUDIO_NO_ERROR) {
					m_b_stream_play = true;
					CHAR sz_palystatus[MAX_PATH];
					sprintf_s(sz_palystatus,
						  "Playing %d channels\0",
						  t_format.m_n_channels);
					SetDlgItemTextA(GetSafeHwnd(),
							IDC_STATIC_PLAY_STATE,
							sz_palystatus);
				} else {
					m_b_stream_play = false;
				}
			}

			m_nPresentChannel = 0;
			for (int i = 0; i < 4; ++i) {
				BOOL bPresent =
					(audioSignalStatus.wChannelValid &
					 (0x01 << i))
						? TRUE
						: FALSE;
				m_nPresentChannel += bPresent ? 2 : 0;
			}
			m_nBitDepthInByte = t_format.m_n_bitspersample / 8;

			this->PostMessageW(WM_REFRESH_CHANNELINFO_MSG);

			m_bGotSyncTag = FALSE;
			m_nBurstSize = -1;
			m_cbBurstHead = 0;
			memset(m_szEncodedFmt, 0, MAX_PATH);
			memset(m_szEncodedFileFmt, 0, MAX_PATH);
			memset(m_byAudioSamples, 0, sizeof(m_byAudioSamples));
			memset(m_byPlaySamples, 0, sizeof(m_byPlaySamples));
			memset(m_byEncodedAudioSamples, 0,
			       sizeof(m_byEncodedAudioSamples));
		}

		if (ullStatusBits & MWCAP_NOTIFY_AUDIO_FRAME_BUFFERED) {
			do {
				if (MW_SUCCEEDED !=
				    MWCaptureAudioFrame(m_hChannel,
							&m_audioFrame))
					break;

				BYTE *pbAudioFrame =
					(BYTE *)m_audioFrame.adwSamples;

				if (m_bIsLPCM) {
					for (int j = 0;
					     j < m_nPresentChannel / 2; ++j) {
						for (int i = 0;
						     i <
						     MWCAP_AUDIO_SAMPLES_PER_FRAME;
						     i++) {
							int nWritePos =
								(i * m_nPresentChannel +
								 j * 2) *
								m_nBitDepthInByte;
							int nWritePosPlay =
								(i * m_n_playchannels +
								 j * 2) *
								m_nBitDepthInByte;
							int nReadPos =
								(i * MWCAP_AUDIO_MAX_NUM_CHANNELS +
								 j) *
								MAX_BIT_DEPTH_IN_BYTE;
							int nReadPos2 =
								(i * MWCAP_AUDIO_MAX_NUM_CHANNELS +
								 j +
								 MWCAP_AUDIO_MAX_NUM_CHANNELS /
									 2) *
								MAX_BIT_DEPTH_IN_BYTE;
							for (int k = 0;
							     k <
							     m_nBitDepthInByte;
							     ++k) {
								m_byAudioSamples[nWritePos +
										 k] = pbAudioFrame
									[nReadPos +
									 MAX_BIT_DEPTH_IN_BYTE -
									 m_nBitDepthInByte +
									 k];
								m_byAudioSamples[nWritePos +
										 m_nBitDepthInByte +
										 k] = pbAudioFrame
									[nReadPos2 +
									 MAX_BIT_DEPTH_IN_BYTE -
									 m_nBitDepthInByte +
									 k];
								if (j <
								    m_n_playchannels /
									    2) {
									m_byPlaySamples[nWritePosPlay +
											k] = pbAudioFrame
										[nReadPos +
										 MAX_BIT_DEPTH_IN_BYTE -
										 m_nBitDepthInByte +
										 k];
									m_byPlaySamples[nWritePosPlay +
											m_nBitDepthInByte +
											k] = pbAudioFrame
										[nReadPos2 +
										 MAX_BIT_DEPTH_IN_BYTE -
										 m_nBitDepthInByte +
										 k];
								}
							}
						}
					}
					if (m_b_render && m_b_dsound &&
					    m_b_play_stream &&
					    m_b_stream_play) {
						m_p_multi_ds_render->put_frame(
							m_byPlaySamples,
							MWCAP_AUDIO_SAMPLES_PER_FRAME *
								m_n_playchannels *
								m_nBitDepthInByte);
					}

					if (m_bRecording && m_bStartRecord &&
					    m_file.IsOpen()) {
						if (m_file.Write(
							    (const BYTE *)
								    m_byAudioSamples,
							    MWCAP_AUDIO_SAMPLES_PER_FRAME *
								    m_nPresentChannel *
								    m_nBitDepthInByte))
							m_nFileSize +=
								MWCAP_AUDIO_SAMPLES_PER_FRAME *
								m_nPresentChannel *
								m_nBitDepthInByte;
					}

					if (m_bDisplay)
						ViewFrame();
				} else {
					int nEncodedFrameSize = 0;
					for (int j = 0; j < 1; ++j) {
						for (int i = 0;
						     i <
						     MWCAP_AUDIO_SAMPLES_PER_FRAME;
						     i++) {
							int nWritePos =
								(i * 2 +
								 j * 2) *
								m_nBitDepthInByte;
							int nReadPos =
								(i * MWCAP_AUDIO_MAX_NUM_CHANNELS +
								 j) *
								MAX_BIT_DEPTH_IN_BYTE;
							int nReadPos2 =
								(i * MWCAP_AUDIO_MAX_NUM_CHANNELS +
								 j +
								 MWCAP_AUDIO_MAX_NUM_CHANNELS /
									 2) *
								MAX_BIT_DEPTH_IN_BYTE;
							for (int k = 0;
							     k <
							     m_nBitDepthInByte;
							     ++k) {
								m_byEncodedAudioSamples
									[nWritePos +
									 k] = pbAudioFrame
										[nReadPos +
										 MAX_BIT_DEPTH_IN_BYTE -
										 k -
										 1];
								m_byEncodedAudioSamples
									[nWritePos +
									 m_nBitDepthInByte +
									 k] = pbAudioFrame
										[nReadPos2 +
										 MAX_BIT_DEPTH_IN_BYTE -
										 k -
										 1];
							}
						}
					}
					ParseEncodedAudioFrame();
				}

			} while (m_bAudioCapturing);
		}
	}
	if (m_b_render && m_b_dsound && m_b_play_stream && m_b_stream_play) {
		m_p_multi_ds_render->stop_stream();
		m_b_stream_play = false;
		m_p_multi_ds_render->destory_stream();
		m_b_play_stream = false;
	}

	MWUnregisterNotify(m_hChannel, hAudioNotify);

	return 0;
};

void CMultiAudioCaptureDlg::OnBnClickedButtonRecord()
{
	if (m_bRecording) {
		m_bRecording = FALSE;
		m_bStartRecord = FALSE;

		if (m_file.IsOpen()) {
			m_file.Exit();
		}

		if (m_pEncodedFile) {
			fclose(m_pEncodedFile);
			m_pEncodedFile = NULL;

			CHAR szNewName[MAX_PATH];
			if (strlen(m_szEncodedFmt) > 0) {
				sprintf_s(szNewName, "%s.%s\0",
					  m_szLastRecordFile,
					  m_szEncodedFileFmt);
			} else {
				sprintf_s(szNewName, "%s%s\0",
					  m_szLastRecordFile,
					  ENCODED_FILE_SUFFIX);
			}
			rename(m_szLastRecordFile, szNewName);
			strcpy_s(m_szLastRecordFile, szNewName);
		}

		KillTimer(UPDATE_FILE_SIZE_TIMER_ID);

		UpdateBottomStatus();

		SetDlgItemText(IDC_BUTTON_RECORD, _T("Record To File "));
	} else {
		do {
			if (m_nPresentChannel <= 0) {
				MessageBox(_T("No audio channel input !\n"),
					   _T("MultiAudioCapture"));
				break;
			}

			if (m_bIsLPCM) {
				time_t timeNow = time(NULL);
				struct tm *tmLocal;
				tmLocal = localtime(&timeNow);
				strcpy_s(m_szLastRecordFile,
					 getenv("HOMEDRIVE"));
				strcat_s(m_szLastRecordFile,
					 getenv("HOMEPATH"));
				strcat_s(m_szLastRecordFile, "\\Magewell");

				_mkdir(m_szLastRecordFile);

				CHAR fileName[128];
				sprintf_s(fileName,
					  "\\%s-%d-%02d-%02d-%02d-%02d-%02d%s",
					  PCM_FILE_NAME,
					  tmLocal->tm_year + 1900,
					  tmLocal->tm_mon + 1, tmLocal->tm_mday,
					  tmLocal->tm_hour, tmLocal->tm_min,
					  tmLocal->tm_sec, PCM_FILE_SUFFIX);
				strcat_s(m_szLastRecordFile, fileName);

				if (!m_file.Init(
					    m_szLastRecordFile,
					    m_audioSignalStatus.dwSampleRate,
					    m_nPresentChannel,
					    m_nBitDepthInByte * 8)) {
					FILE *wavFile = NULL;
					errno_t errNum = fopen_s(
						&wavFile, m_szLastRecordFile,
						"wb");
					if (NULL == wavFile) {
						if (errNum == 13)
							MessageBox(
								_T("Record wav file fail !\nError: Permission denied!\n"),
								_T("MultiAudioCapture"));
					} else {
						fclose(wavFile);
						wavFile = NULL;
						MessageBox(
							_T("Record wav file fail !\n"),
							_T("MultiAudioCapture"));
					}

					break;
				}
				m_bStartRecord = TRUE;
			} else {
				time_t timeNow = time(NULL);
				struct tm *tmLocal;
				tmLocal = localtime(&timeNow);

				strcpy_s(m_szLastRecordFile,
					 getenv("HOMEDRIVE"));
				strcat_s(m_szLastRecordFile,
					 getenv("HOMEPATH"));
				strcat_s(m_szLastRecordFile, "\\Magewell");

				_mkdir(m_szLastRecordFile);

				CHAR fileName[128];
				sprintf_s(fileName,
					  "\\%s-%d-%02d-%02d-%02d-%02d-%02d",
					  ENCODED_FILE_NAME,
					  tmLocal->tm_year + 1900,
					  tmLocal->tm_mon + 1, tmLocal->tm_mday,
					  tmLocal->tm_hour, tmLocal->tm_min,
					  tmLocal->tm_sec);
				strcat_s(m_szLastRecordFile, fileName);

				if (0 != fopen_s(&m_pEncodedFile,
						 m_szLastRecordFile, "wb")) {
					MessageBox(
						_T("Record encoded audio file fail !\n"),
						_T("MultiAudioCapture"));
					break;
				}
				m_bStartRecord = FALSE;
			}

			m_nFileSize = 0;
			m_bRecording = TRUE;

			SetDlgItemText(IDC_BUTTON_RECORD, _T("Stop"));
			GetDlgItem(IDC_BUTTON_PLAY)->EnableWindow(TRUE);

			SetTimer(UPDATE_FILE_SIZE_TIMER_ID, 500, NULL);
		} while (FALSE);
	}
}

void CMultiAudioCaptureDlg::OnBnClickedButtonPlay()
{
	if (m_bRecording) {
		//MessageBox(_T("Please stop recording first !\n"), _T("MultiAudioCapture"));
		OnBnClickedButtonRecord();
	}

	if (strlen(m_szLastRecordFile) > 0) {
		int nError = (int)ShellExecuteA(
			NULL, "open", m_szLastRecordFile, NULL, NULL, SW_HIDE);
		if (nError < 32)
			MessageBox(L"Error: File open failed!\n");
	}
}

void CMultiAudioCaptureDlg::OnClose()
{
	UninitChart();

	if (m_bAudioCapturing) {
		m_bAudioCapturing = FALSE;
		if (m_hAudioEvent)
			SetEvent(m_hAudioEvent);
		WaitForSingleObject(m_hAudioCaptureThread, INFINITE);
		CloseHandle(m_hAudioCaptureThread);
		m_hAudioCaptureThread = NULL;
	}

	if (m_bRecording) {
		OnBnClickedButtonRecord();
	}

	if (m_hChannel) {
		if (MW_SUCCEEDED != MWStopAudioCapture(m_hChannel))
			OutputDebugStringA(
				"CMultiAudioCaptureDlg::Stop audio capture return error !");

		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	if (m_hAudioEvent) {
		CloseHandle(m_hAudioEvent);
		m_hAudioEvent = NULL;
	}

	if (m_p_multi_ds_render != NULL) {
		m_p_multi_ds_render->deinitilize_dsound();
		m_b_dsound = false;
		delete m_p_multi_ds_render;
		m_p_multi_ds_render = NULL;
		m_b_render = false;
	}

	if (m_h_reset_device_event != NULL) {
		CloseHandle(m_h_reset_device_event);
		m_h_reset_device_event = NULL;
	}

	CDialogEx::OnClose();
}

void CMultiAudioCaptureDlg::InitChart()
{
	TCHAR szName[MAX_PATH];

	for (int i = 0; i < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++i) {

		CChartAxis *pAxisHor =
			m_chart[i].CreateStandardAxis(CChartCtrl::BottomAxis);
		pAxisHor->SetAutomatic(true);
		pAxisHor->SetVisible(false);

		CChartAxis *pAxisVer =
			m_chart[i].CreateStandardAxis(CChartCtrl::LeftAxis);
		pAxisVer->SetMinMax(-32768, 32768);
		pAxisVer->SetAutomatic(false);
		pAxisVer->SetVisible(false);

		m_chart[i].SetBorderColor(0x00777777);
		m_chart[i].SetBackColor(0x00777777);

		m_chart[i].EnableRefresh(false);

		m_pLine[i] = m_chart[i].CreateLineSerie();
		m_pLine[i]->SetSeriesOrdering(poXOrdering);
		m_pLine[i]->SetColor(0x007FFF7F);

		_stprintf_s(szName, _T("Audio channel %d"), i);
		m_pLine[i]->SetName(szName);
		m_pLine[i]->SetShadowDepth(0);
	}

	m_szFrameX[UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME - 1] = 0;
	m_nUpdatePos = 0;

	SetTimer(RENDER_TIMER_ID, 300, NULL);
}

void CMultiAudioCaptureDlg::UninitChart()
{
	KillTimer(RENDER_TIMER_ID);
}

void CMultiAudioCaptureDlg::ViewFrame()
{
	m_mutexChart.Lock();

	m_szFrameX[m_nUpdatePos] =
		m_szFrameX[(m_nUpdatePos +
			    UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME - 1) %
			   (UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME)] +
		1;

	for (int i = 0; i < VIEW_SAMPLES_PER_FRAME; ++i) {
		if (i < VIEW_SAMPLES_PER_FRAME - 1)
			m_szFrameX[m_nUpdatePos + i + 1] =
				m_szFrameX[m_nUpdatePos + i] + 1;

		for (int j = 0; j < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++j) {
			if (j >= m_nPresentChannel) {
				m_szFrameY[j][m_nUpdatePos + i] = 0;
				continue;
			}

			int nReadPos =
				(i * VIEW_PERCENT * m_nPresentChannel + j) *
				m_nBitDepthInByte;
			DWORD dwValue = 0;
			for (int k = 0; k < m_nBitDepthInByte; ++k) {
				dwValue += (m_byAudioSamples[nReadPos + k]
					    << (k * 8));
			}
			//m_szFrameY[j][m_nUpdatePos + i] = (short)dwValue;
			if (dwValue >=
			    ((DWORD)0x01 << (m_nBitDepthInByte * 8 - 1))) {
				m_szFrameY[j][m_nUpdatePos + i] =
					(LONGLONG)dwValue -
					((LONGLONG)(0x01)
					 << m_nBitDepthInByte * 8);
			} else {
				m_szFrameY[j][m_nUpdatePos + i] = dwValue;
			}
		}
	}
	m_nUpdatePos += VIEW_SAMPLES_PER_FRAME;

	if (m_nUpdatePos == UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME) {
		m_nUpdatePos = 0;
		this->PostMessageW(WM_REFRESH_LINESERIE_MSG);
	}

	m_mutexChart.Unlock();
}

void CMultiAudioCaptureDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent) {
	case RENDER_TIMER_ID: {
		//UpdateWaveView();
		break;
	}
	case UPDATE_FILE_SIZE_TIMER_ID: {
		UpdateBottomStatus();
		break;
	}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CMultiAudioCaptureDlg::UpdateBottomStatus()
{
	if (m_bRecording)
		sprintf_s(m_szBottomStatus, "Writting %s file ... ",
			  m_bIsLPCM ? "wave" : m_szEncodedFmt);
	else
		sprintf_s(m_szBottomStatus, "Audio %s file saved",
			  m_bIsLPCM ? "wave" : m_szEncodedFmt);

	if (m_nFileSize >= 1024 * 1024) {
		sprintf_s(m_szBottomStatus, "%s %.2f MB", m_szBottomStatus,
			  (double)m_nFileSize / (1024 * 1024));
	} else if (m_nFileSize >= 1024) {
		sprintf_s(m_szBottomStatus, "%s %.2f KB", m_szBottomStatus,
			  (double)m_nFileSize / 1024);
	} else {
		sprintf_s(m_szBottomStatus, "%s %d B", m_szBottomStatus,
			  m_nFileSize);
	}

	SetDlgItemTextA(GetSafeHwnd(), IDC_STATIC_BOTTOM_STATUS,
			m_szBottomStatus);
}

void CMultiAudioCaptureDlg::UpdateWaveView()
{
	for (int j = 0; j < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++j) {
		m_chart[j].EnableRefresh(true);
		m_chart[j].EnableRefresh(false);
	}
}

void CMultiAudioCaptureDlg::ParseEncodedAudioFrame()
{
	int nTotallSize = m_nBitDepthInByte * 2 * MWCAP_AUDIO_SAMPLES_PER_FRAME;
	int nReadSize = 0;

	while (nReadSize < nTotallSize) {
		// Write payload
		if (m_nBurstSize > 0) {
			int nValid = min(m_nBurstSize, nTotallSize - nReadSize);
			m_nBurstSize -= nValid;

			if (m_bRecording && m_bStartRecord && m_pEncodedFile) {
				if (1 ==
				    fwrite(m_byEncodedAudioSamples + nReadSize,
					   nValid, 1, m_pEncodedFile))
					m_nFileSize += nValid;
			}
			nReadSize += nValid;
		}

		if (m_nBurstSize > 0)
			continue;

		// Find next sync tag "F8 72 4E 1F"
		for (; (nReadSize < nTotallSize) && !m_bGotSyncTag;
		     ++nReadSize) {
			if (m_byEncodedAudioSamples[nReadSize] == 0xf8 &&
			    m_byEncodedAudioSamples[nReadSize + 1] == 0x72 &&
			    m_byEncodedAudioSamples[nReadSize + 2] == 0x4e &&
			    m_byEncodedAudioSamples[nReadSize + 3] == 0x1f) {
				m_bGotSyncTag = true;
				nReadSize += 2 * sizeof(WORD);
				break;
			}
		}

		if (!m_bGotSyncTag)
			continue;

		// Collect another 4 bytes in burst header
		int nValid = min(nTotallSize - nReadSize, 4 - m_cbBurstHead);
		memcpy(m_byBurstHead, m_byEncodedAudioSamples + nReadSize,
		       nValid);
		m_cbBurstHead += nValid;
		nReadSize += nValid;

		if (m_cbBurstHead != 4)
			continue;

		// Parse burst head to get audio format and burst size
		if (strlen(m_szEncodedFmt) == 0) {
			switch (m_byBurstHead[1] & 0x7f) {
			case 0x01:
				strcpy_s(m_szEncodedFmt, "AC-3");
				strcpy_s(m_szEncodedFileFmt, "ac3");
				break;
			case 0x04:
				strcpy_s(m_szEncodedFmt, "MPEG1");
				strcpy_s(m_szEncodedFileFmt, "mpeg1");
				break;
			case 0x05:
				strcpy_s(m_szEncodedFmt, "MP3");
				strcpy_s(m_szEncodedFileFmt, "mp3");
				break;
			case 0x06:
				strcpy_s(m_szEncodedFmt, "MPEG2");
				strcpy_s(m_szEncodedFileFmt, "mp2");
				break;
			case 0x07:
				strcpy_s(m_szEncodedFmt, "MPEG2-AAC");
				strcpy_s(m_szEncodedFileFmt, "aac");
				break;
			case 0x08:
				strcpy_s(m_szEncodedFmt, "MPEG2-L1");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_l1");
				break;
			case 0x09:
				strcpy_s(m_szEncodedFmt, "MPEG2-L2");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_l2");
				break;
			case 0x0a:
				strcpy_s(m_szEncodedFmt, "MPEG2-L3");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_l3");
				break;
			case 0x0b:
				strcpy_s(m_szEncodedFmt, "DTS-TYPE1");
				strcpy_s(m_szEncodedFileFmt, "dts");
				break;
			case 0x0c:
				strcpy_s(m_szEncodedFmt, "DTS-TYPE2");
				strcpy_s(m_szEncodedFileFmt, "dts");
				break;
			case 0x0d:
				strcpy_s(m_szEncodedFmt, "DTS-TYPE3");
				strcpy_s(m_szEncodedFileFmt, "dts");
				break;
			case 0x0e:
				strcpy_s(m_szEncodedFmt, "ATARC");
				strcpy_s(m_szEncodedFileFmt, "atarc");
				break;
			case 0x0f:
				strcpy_s(m_szEncodedFmt, "ATARC 2/3");
				strcpy_s(m_szEncodedFileFmt, "atarc");
				break;
			case 0x10:
				strcpy_s(m_szEncodedFmt, "ATARC-X");
				strcpy_s(m_szEncodedFileFmt, "atarc");
				break;
			case 0x11:
				strcpy_s(m_szEncodedFmt, "DTS-TYPE4");
				strcpy_s(m_szEncodedFileFmt, "dts");
				break;
			case 0x12:
				strcpy_s(m_szEncodedFmt, "WMA-TYPE1");
				strcpy_s(m_szEncodedFileFmt, "wma");
				break;
			case 0x32:
				strcpy_s(m_szEncodedFmt, "WMA-TYPE2");
				strcpy_s(m_szEncodedFileFmt, "wma");
				break;
			case 0x52:
				strcpy_s(m_szEncodedFmt, "WMA-TYPE3");
				strcpy_s(m_szEncodedFileFmt, "wma");
				break;
			case 0x72:
				strcpy_s(m_szEncodedFmt, "WMA-TYPE4");
				strcpy_s(m_szEncodedFileFmt, "wma");
				break;
			case 0x13:
				strcpy_s(m_szEncodedFmt, "MPEG2-AAC LF");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_aac");
				break;
			case 0x33:
				strcpy_s(m_szEncodedFmt, "MPEG2-AAC LF");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_aac");
				break;
			case 0x53:
				strcpy_s(m_szEncodedFmt, "MPEG2-AAC LF");
				strcpy_s(m_szEncodedFileFmt, "mpeg2_aac");
				break;
			case 0x73:
				strcpy_s(m_szEncodedFmt, "MPEG2-AAC LF");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac");
				break;
			case 0x14:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac");
				break;
			case 0x34:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac");
				break;
			case 0x54:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac");
				break;
			case 0x74:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac");
				break;
			case 0x15:
				strcpy_s(m_szEncodedFmt, "Enhanced AC-3");
				strcpy_s(m_szEncodedFileFmt, "eac3");
				break;
			case 0x16:
				strcpy_s(m_szEncodedFmt, "MAT");
				strcpy_s(m_szEncodedFileFmt, "mat");
				break;
			case 0x17:
				strcpy_s(m_szEncodedFmt, "MPEG4-ALS");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_als");
				break;
			case 0x37:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC-LC");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac_lc");
				break;
			case 0x57:
				strcpy_s(m_szEncodedFmt, "MPEG4-AAC-HE");
				strcpy_s(m_szEncodedFileFmt, "mpeg4_aac_he");
				break;
			case 0x77:
				strcpy_s(m_szEncodedFmt, "DRA");
				strcpy_s(m_szEncodedFileFmt, "dra");
				break;
			case 0x18:
				strcpy_s(m_szEncodedFmt, "AC-4");
				strcpy_s(m_szEncodedFileFmt, "ac4");
				break;
			case 0x38:
				strcpy_s(m_szEncodedFmt, "AC-4-HBR4");
				strcpy_s(m_szEncodedFileFmt, "ac4");
				break;
			case 0x58:
				strcpy_s(m_szEncodedFmt, "AC-4-HBR16");
				strcpy_s(m_szEncodedFileFmt, "ac4");
				break;
			case 0x78:
				strcpy_s(m_szEncodedFmt, "AC-4-LD");
				strcpy_s(m_szEncodedFileFmt, "ac4");
				break;
			case 0x19:
				strcpy_s(m_szEncodedFmt, "MPEGH-3D");
				strcpy_s(m_szEncodedFileFmt, "mpegh_3d");
				break;
			case 0x39:
				strcpy_s(m_szEncodedFmt, "MPEGH-3D-HBR");
				strcpy_s(m_szEncodedFileFmt, "mpegh_3d_hbr");
				break;
			case 0x59:
				strcpy_s(m_szEncodedFmt, "MPEG4-ALS-LATM/LOAS");
				strcpy_s(m_szEncodedFileFmt,
					 "mpeg4_als_latm/loas");
				break;
			}

			if (strlen(m_szEncodedFmt) > 0) {
				SetDlgItemTextA(GetSafeHwnd(),
						IDC_STATIC_AUDIO_FORMAT,
						m_szEncodedFmt);
				this->PostMessageW(WM_REFRESH_CHANNELINFO_MSG);
			}
		}

		m_nBurstSize = (((WORD)m_byBurstHead[2] << 8) +
				(WORD)m_byBurstHead[3]) /
			       8;
		switch (m_byBurstHead[1] & 0x7f) {
		case 0x11:
		case 0x15:
		case 0x16:
			m_nBurstSize *= 8;
			break;
		case 0x17:
		case 0x58:
		case 0x39:
		case 0x59:
			m_nBurstSize *= 64;
			break;
		}

		m_cbBurstHead = 0;
		m_bGotSyncTag = FALSE;

		// Start recording from the begining of a new audio frame
		if (m_bRecording && !m_bStartRecord && m_pEncodedFile) {
			m_bStartRecord = TRUE;
		}
	}
}

void CMultiAudioCaptureDlg::OnBnClickedCheckWave()
{
	UpdateData();
}

LRESULT CMultiAudioCaptureDlg::OnRefreshLineSerieHandle(WPARAM, LPARAM)
{
	m_mutexChart.Lock();

	for (int j = 0; j < MWCAP_AUDIO_MAX_NUM_CHANNELS; ++j) {
		if (m_pLine[j]->GetPointsCount() >=
		    VIEW_SAMPLES_PER_FRAME * VIEW_LENGTH) {
			m_pLine[j]->RemovePointsFromBegin(
				UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME);
		}

		m_pLine[j]->AddPoints(m_szFrameX, m_szFrameY[j],
				      UPDATE_LENGTH * VIEW_SAMPLES_PER_FRAME);
	}

	UpdateWaveView();

	m_mutexChart.Unlock();

	return NULL;
}

LRESULT CMultiAudioCaptureDlg::OnRefreshChannelInfoHandle(WPARAM, LPARAM)
{
	UpdateInfo();

	return 1;
}

BOOL CMultiAudioCaptureDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN))

		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CMultiAudioCaptureDlg::OnCbnSelchangeChannelCombo()
{
	CString channelName;
	int nSel;

	nSel = m_coChannelName.GetCurSel();
	m_coChannelName.GetLBText(nSel, channelName);

	if (m_bAudioCapturing) {
		m_bAudioCapturing = FALSE;
		if (m_hAudioEvent)
			SetEvent(m_hAudioEvent);
		WaitForSingleObject(m_hAudioCaptureThread, INFINITE);
		CloseHandle(m_hAudioCaptureThread);
		m_hAudioCaptureThread = NULL;
	}

	if (m_bRecording) {
		OnBnClickedButtonRecord();
	}

	if (m_hChannel) {
		if (MW_SUCCEEDED != MWStopAudioCapture(m_hChannel))
			OutputDebugStringA(
				"CMultiAudioCaptureDlg::Stop audio capture return error !");

		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	if (m_hAudioEvent) {
		CloseHandle(m_hAudioEvent);
		m_hAudioEvent = NULL;
	}

	WCHAR szDevicePath[MAX_PATH];
	::MWGetDevicePath(g_nValidChannel[nSel], szDevicePath);

	m_hChannel = ::MWOpenChannelByPath(szDevicePath);

	DWORD dwInputCount = 0;
	MWGetAudioInputSourceArray(m_hChannel, NULL, &dwInputCount);

	UpdateInfo();

	m_hAudioEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (MW_SUCCEEDED != MWStartAudioCapture(m_hChannel)) {
		MessageBox(_T("Start capture audio fail !"),
			   _T("MultiAudioCapture"));
		return;
	}

	m_bAudioCapturing = TRUE;
	m_hAudioCaptureThread = CreateThread(NULL, 0, AudioCaptureThreadProc,
					     (LPVOID)this, 0, NULL);
	if (m_hAudioCaptureThread == NULL) {
		m_bAudioCapturing = FALSE;
		return;
	}
}

BOOL CAboutDlg::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN) &&
	    pMsg->wParam)

		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
