/************************************************************************************************/
// NDISenderDlg.cpp : implementation of the CNDISenderDlg class

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
#include "NDISender.h"
#include "NDISenderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define ID_VIDEO_WND 1005
int g_dwResolution[3][2] = {{1920, 1080}, {1280, 720}, {720, 480}};

DWORD g_nFPS[2] = {30, 60};

char g_szFourcc[10][256] = {"BGRA", "UYVY", NULL};

DWORD g_dwFourcc[10] = {MWFOURCC_BGRA, MWFOURCC_UYVY, NULL};

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange *pDX); // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CNDISenderDlg 对话框

CNDISenderDlg::CNDISenderDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CNDISenderDlg::IDD, pParent),
	  m_strSignal(_T("")),
	  m_strResolution(_T("")),
	  m_strTotalSize(_T("")),
	  m_strOffset(_T("")),
	  m_strAspectRatio(_T("")),
	  m_strColorFormat(_T("")),
	  m_strAudioFormat(_T(""))
//, m_strTimeStatus(_T(""))
//, m_strTime(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_nChannelIndex = -1;
	m_nSelectIndex = -1;
	m_nFormatSelectIndex = -1;
	m_hChannel = NULL;
	m_bShowTime = FALSE;
	m_bUpdateChannel = FALSE;
}

void CNDISenderDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMBCHANNEL, m_cmbDevice);
	DDX_Text(pDX, IDC_STATIC_SIGNAL, m_strSignal);
	DDX_Text(pDX, IDC_STATIC_RESOLUTION, m_strResolution);
	DDX_Text(pDX, IDC_STATIC_TOTALSCAN, m_strTotalSize);
	DDX_Text(pDX, IDC_STATIC_OFFSET, m_strOffset);
	DDX_Control(pDX, IDC_CMBCAPTUREFORMAT, m_cmbCaptureFormat);
	DDX_Text(pDX, IDC_STATIC_ASPECTRATIO, m_strAspectRatio);
	DDX_Text(pDX, IDC_STATIC_COLORFORMAT, m_strColorFormat);
	DDX_Text(pDX, IDC_STATIC_AUDIOFORMAT, m_strAudioFormat);
	DDX_Control(pDX, ID_BTNSTOP, m_btnStop);
	DDX_Control(pDX, IDC_BTNSTART, m_btnStart);
	DDX_Control(pDX, IDC_SENDER_NAME, m_edtSendName);
	//DDX_Text(pDX, IDC_STATIC_TIMESTATUS, m_strTimeStatus);
	//DDX_Text(pDX, IDC_STATIC_TIME, m_strTime);
}

BEGIN_MESSAGE_MAP(CNDISenderDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()

//add
ON_WM_TIMER()
ON_WM_DESTROY()
ON_CBN_SELCHANGE(IDC_CMBCHANNEL, &CNDISenderDlg::OnCbnSelchangeCombo0)
ON_CBN_SELCHANGE(IDC_CMBCAPTUREFORMAT,
		 &CNDISenderDlg::OnCbnSelchangeComboxFormat)
ON_WM_MOVE()

ON_BN_CLICKED(ID_BTNEXIT, &CNDISenderDlg::OnBnClickedBtnexit)
ON_BN_CLICKED(IDC_BTNSTART, &CNDISenderDlg::OnBnClickedBtnstart)
ON_BN_CLICKED(ID_BTNSTOP, &CNDISenderDlg::OnBnClickedBtnstop)
END_MESSAGE_MAP()

// CNDISenderDlg 消息处理程序

BOOL CNDISenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);  // 设置大图标
	SetIcon(m_hIcon, FALSE); // 设置小图标

	// TODO: 在此添加额外的初始化代码

	//create render
	CRect rcPos;
	CWnd *pWnd;

	pWnd = GetDlgItem(IDC_PIC_PREVIEW);
	pWnd->GetWindowRect(rcPos);
	ScreenToClient(rcPos);
	pWnd->ShowWindow(SW_HIDE);
	m_videoWnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcPos, this,
			  ID_VIDEO_WND);
	m_videoSend.Create(NULL, NULL, WS_CHILD, rcPos, this, ID_VIDEO_WND);

	TCHAR szFormatName[MAX_PATH];
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				int num = (i * 4 + j * 2 + k);
				m_format[num].cx = g_dwResolution[i][0];
				m_format[num].cy = g_dwResolution[i][1];
				m_format[num].dwFourcc = g_dwFourcc[j];
				m_format[num].szFormat = g_szFourcc[j];
				m_format[num].fps = g_nFPS[k];

				CAutoConvertString strName(g_szFourcc[j]);
				_stprintf_s(szFormatName,
					    _T("%d x %d, %s, %d fps\0"),
					    g_dwResolution[i][0],
					    g_dwResolution[i][1],
					    (const TCHAR *)strName, g_nFPS[k]);
				int nItem = m_cmbCaptureFormat.InsertString(
					(num), szFormatName);
				m_cmbCaptureFormat.SetItemData(
					nItem, (DWORD_PTR)(num));
			}
		}
	}

	m_cmbCaptureFormat.SetCurSel(0);
	m_nFormatSelectIndex = 0;

	//insert combox device
	//int nItemDevice;
	//nItemDevice = m_cmbDevice.InsertString(0, _T("(No select channel)"));
	//m_cmbDevice.SetItemData(nItemDevice, -1);

	TCHAR szChannelName[MAX_PATH];
	for (int i = 0; i < g_nValidChannelCount; i++) {
		MWCAP_CHANNEL_INFO chlInfo;
		if (MW_SUCCEEDED ==
		    MWGetChannelInfoByIndex(g_nValidChannel[i], &chlInfo)) {
			CAutoConvertString strName(chlInfo.szProductName);
			_stprintf_s(szChannelName, _T("%s %d-%d\0"),
				    (const TCHAR *)strName,
				    chlInfo.byBoardIndex,
				    chlInfo.byChannelIndex);
			int nItem = m_cmbDevice.InsertString(i, szChannelName);
			m_cmbDevice.SetItemData(nItem, (DWORD_PTR)(i));
		}
	}

	m_cmbDevice.SetCurSel(0);
	m_videoWnd.OpenChannel(0, CAPTURE_WIDTH, CAPTURE_HEIGHT,
			       CAPTURE_DURATION, CAPTURE_COLOR);
	m_nChannelIndex = 0;
	m_nSelectIndex = 0;

	m_btnStop.EnableWindow(FALSE);
	m_bUpdateChannel = TRUE;
	UpdateChannel();
	m_bUpdateChannel = FALSE;

	SetDlgItemText(IDC_SENDER_NAME, _T("NDISender"));

	SetTimer(1, 100, NULL);
	return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

void CNDISenderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	} else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNDISenderDlg::OnPaint()
{
	if (IsIconic()) {
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND,
			    reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	} else {
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNDISenderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNDISenderDlg::OnDestroy()
{
	KillTimer(1);

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	m_videoWnd.CloseChannel();
	m_videoSend.DestoryNDISender();
	CDialogEx::OnDestroy();
}

BOOL CNDISenderDlg::UpdateChannel()
{
	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	if (m_nChannelIndex != -1) {
		WCHAR devicePath[256];
		memset(devicePath, 0, 256);
		MW_RESULT xr = MWGetDevicePath(g_nValidChannel[m_nChannelIndex],
					       devicePath);
		if (xr != MW_SUCCEEDED)
			return FALSE;

		m_hChannel = MWOpenChannelByPath(devicePath);
		if (m_hChannel == NULL)
			return FALSE;
	}

	return TRUE;
}

void CNDISenderDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (m_bUpdateChannel == FALSE) {
		if (m_nChannelIndex != -1) {
			MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
			MW_RESULT xr = MWGetVideoSignalStatus(
				m_hChannel, &videoSignalStatus);
			if (xr != MW_SUCCEEDED)
				UpdateData(TRUE);

			MWCAP_AUDIO_SIGNAL_STATUS audioSignalStatus;
			xr = MWGetAudioSignalStatus(m_hChannel,
						    &audioSignalStatus);
			if (xr != MW_SUCCEEDED)
				UpdateData(TRUE);

			int nSel0 = m_cmbDevice.GetCurSel();
			if (nSel0 == -1 || videoSignalStatus.state !=
						   MWCAP_VIDEO_SIGNAL_LOCKED) {
				m_strSignal = _T("-");
				m_strResolution = _T("-");
				m_strTotalSize = _T("-");
				m_strOffset = _T("-");
				m_strColorFormat = _T("-");
				m_strAspectRatio = _T("-");
				m_strAudioFormat = _T("-");
			} else {
				m_strSignal =
					GetStatus(videoSignalStatus.state);
				m_strResolution = GetResolutions(
					videoSignalStatus.cx,
					videoSignalStatus.cy,
					videoSignalStatus.dwFrameDuration,
					videoSignalStatus.bInterlaced);
				m_strTotalSize =
					GetScanSize(videoSignalStatus.cxTotal,
						    videoSignalStatus.cyTotal);
				m_strOffset = GetOffset(videoSignalStatus.x,
							videoSignalStatus.y);
				m_strAspectRatio = GetAspectRatio(
					videoSignalStatus.nAspectX,
					videoSignalStatus.nAspectY);
				m_strColorFormat = GetColorFormat(
					videoSignalStatus.colorFormat);
				m_strAudioFormat = GetAudioFormat(
					audioSignalStatus.bLPCM,
					audioSignalStatus.cBitsPerSample,
					audioSignalStatus.dwSampleRate);
			}
			UpdateData(FALSE);

		} else {
			m_strSignal = _T("-");
			m_strResolution = _T("-");
			m_strTotalSize = _T("-");
			m_strOffset = _T("-");
			UpdateData(FALSE);
		}
	}

	if (m_bShowTime == TRUE) {
		CTime currentTime;
		currentTime = CTime::GetCurrentTime();

		CTimeSpan temp = currentTime - m_time;
		CString title;
		title.Format(_T("NDISender-%02d:%02d:%02d"), temp.GetHours(),
			     temp.GetMinutes(), temp.GetSeconds());
		SetWindowText(title);
	} else {
		CString title;
		title.Format(_T("NDISender"));
		SetWindowText(title);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CNDISenderDlg::OnCbnSelchangeCombo0()
{
	int nSel = m_cmbDevice.GetCurSel();
	if (nSel == -1)
		return;
	if (nSel == m_nSelectIndex)
		return;

	m_videoWnd.CloseChannel();
	m_videoSend.DestoryNDISender();
	m_bUpdateChannel = TRUE;

	int nIndex = (int)m_cmbDevice.GetItemData(nSel);
	m_nSelectIndex = nSel;
	m_nChannelIndex = nIndex;
	if (nIndex != -1) {
		if (m_nFormatSelectIndex != -1) {
			m_videoWnd.OpenChannel(
				nIndex, m_format[m_nFormatSelectIndex].cx,
				m_format[m_nFormatSelectIndex].cy,
				1e7 / m_format[m_nFormatSelectIndex].fps,
				m_format[m_nFormatSelectIndex].dwFourcc);
			m_btnStart.EnableWindow(TRUE);
			m_btnStop.EnableWindow(FALSE);
		}
	} else {
		m_btnStart.EnableWindow(FALSE);
		m_btnStop.EnableWindow(FALSE);
	}

	UpdateChannel();
	m_bUpdateChannel = FALSE;
}

void CNDISenderDlg::OnCbnSelchangeComboxFormat()
{
	int nSel = m_cmbCaptureFormat.GetCurSel();
	if (nSel == -1)
		return;
	if (nSel == m_nFormatSelectIndex)
		return;

	m_videoWnd.CloseChannel();
	m_videoSend.DestoryNDISender();

	m_nFormatSelectIndex = nSel;
	if (m_nFormatSelectIndex != -1 && m_nChannelIndex != -1) {
		m_videoWnd.OpenChannel(m_nChannelIndex,
				       m_format[m_nFormatSelectIndex].cx,
				       m_format[m_nFormatSelectIndex].cy,
				       1e7 / m_format[m_nFormatSelectIndex].fps,
				       m_format[m_nFormatSelectIndex].dwFourcc);

		m_btnStart.EnableWindow(TRUE);
		m_btnStop.EnableWindow(FALSE);
	}
}

void CNDISenderDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
}

void CNDISenderDlg::OnBnClickedBtnexit()
{
	SendMessage(WM_CLOSE);
}

void CNDISenderDlg::OnBnClickedBtnstart()
{
	if (m_nChannelIndex != -1) {
		CString strSenderName;
		GetDlgItemText(IDC_SENDER_NAME, strSenderName);
		int len = WideCharToMultiByte(CP_ACP, 0, strSenderName, -1,
					      NULL, 0, NULL, NULL);
		char *pName = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, strSenderName, -1, pName, len,
				    NULL, NULL);

		int nIndex = (int)m_cmbCaptureFormat.GetItemData(
			m_nFormatSelectIndex);
		if (nIndex != -1) {
			m_videoWnd.CloseChannel();
			m_videoSend.DestoryNDISender();
			m_videoSend.CreateNDISender(
				m_nChannelIndex,
				m_format[m_nFormatSelectIndex].cx,
				m_format[m_nFormatSelectIndex].cy,
				1e7 / m_format[m_nFormatSelectIndex].fps,
				m_format[m_nFormatSelectIndex].dwFourcc, pName);
			m_videoWnd.m_bNDISend = TRUE;
			m_videoWnd.CreateNDISendThread();
			m_videoSend.GetNDIVar(&m_videoWnd.m_pNDI_send,
					      &m_videoWnd.m_videoFrame,
					      &m_videoWnd.m_audioFrame);
			m_videoWnd.OpenChannel(
				m_nChannelIndex,
				m_format[m_nFormatSelectIndex].cx,
				m_format[m_nFormatSelectIndex].cy,
				1e7 / m_format[m_nFormatSelectIndex].fps,
				m_format[m_nFormatSelectIndex].dwFourcc);

			m_bShowTime = TRUE;

			m_btnStart.EnableWindow(FALSE);
			m_btnStop.EnableWindow(TRUE);

			m_cmbCaptureFormat.EnableWindow(FALSE);
			m_cmbDevice.EnableWindow(FALSE);

			m_edtSendName.EnableWindow(FALSE);

			m_time = CTime::GetCurrentTime();
		}
	}
}

void CNDISenderDlg::OnBnClickedBtnstop()
{
	m_videoWnd.m_bNDISend = FALSE;
	m_bShowTime = FALSE;

	if (m_btnStart.IsWindowEnabled() == FALSE) {
		m_btnStop.EnableWindow(FALSE);
		m_btnStart.EnableWindow(TRUE);
	}

	if (m_edtSendName.IsWindowEnabled() == FALSE)
		m_edtSendName.EnableWindow(TRUE);

	m_cmbCaptureFormat.EnableWindow(TRUE);
	m_cmbDevice.EnableWindow(TRUE);

	m_videoWnd.DestoryNDISendThread();
}

BOOL CNDISenderDlg::PreTranslateMessage(MSG *pMsg)
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
