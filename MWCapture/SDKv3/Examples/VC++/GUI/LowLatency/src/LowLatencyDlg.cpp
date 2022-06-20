/************************************************************************************************/
// LowLatencyDlg.cpp : implementation file

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
#include "LowLatency.h"
#include "LowLatencyDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#if _MSC_VER >= 1900
#include "stdio.h"
_ACRTIMP_ALT FILE *__cdecl __acrt_iob_func(unsigned);
#ifdef __cplusplus
extern "C"
#endif
	FILE *__cdecl __iob_func(unsigned i)
{
	return __acrt_iob_func(i);
}
#endif

#define ID_VIDEO_WND 1001

// CAboutDlg dialog used for App About

void CLowLatencyDlg::InitJPEGfile(BYTE *pBuffer)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *outfile;           /* target file */
	JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
	int row_stride;          /* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen("LowlatencyNormal.jpeg", "wb")) == NULL) {
		fprintf(stderr, "can't open %s\n", "LowlatencyNormal.jpeg");
		AfxMessageBox(
			L"Can't open LowlatencyNormal.jpeg.Please restart the application with administrator permission.");
		exit(1);
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width =
		CAPTURE_WIDTH; /* image width and height, in pixels */
	cinfo.image_height = CAPTURE_HEIGHT;
	cinfo.input_components =
		BIT_DEPTH; /* # of color components per pixel */
	cinfo.in_color_space = JCS_EXT_RGBA; /* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, 50, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);
	row_stride = CAPTURE_WIDTH *
		     BIT_DEPTH; /* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = &pBuffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
}
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
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD) {}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CLowLatencyDlg dialog

CLowLatencyDlg::CLowLatencyDlg(CWnd *pParent /*=NULL*/)
	: CDialogEx(CLowLatencyDlg::IDD, pParent),
	  m_strSignal0(_T("")),
	  m_strResolution0(_T("")),
	  m_strScanSize0(_T("")),
	  m_strOffset0(_T("")),
	  m_strFormat0(_T("")),
	  m_strCaptureLatency0(_T("")),
	  m_strTotalLatency0(_T("")),
	  m_strHandleLatency0(_T("")),
	  m_strHandleLatency1(_T("")),
	  m_strSignal1(_T("")),
	  m_strResolution1(_T("")),
	  m_strScanSize1(_T("")),
	  m_strOffset1(_T("")),
	  m_strFormat1(_T("")),
	  m_strCaptureLatency1(_T("")),
	  m_strTotalLatency1(_T(""))
//, m_strCurrentFps0(_T(""))
//, m_strCurrentFps1(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < 2; i++) {
		m_data[i].hChannel = NULL;
		m_data[i].hEvent = NULL;
		m_data[i].hThread = NULL;

		m_data[i].status.state = MWCAP_VIDEO_SIGNAL_NONE;
	}

	for (int i = 0; i < 5; i++) {
		m_DataNormal[i].pBuffer = NULL;
	}
	for (int i = 0; i < 30; i++) {
		m_DataLowlatency[i].pBuffer = NULL;
	}
	m_DataJpeg[0].hThread = NULL;
	m_DataJpeg[1].hThread = NULL;
	m_DataJpeg[0].bRun = TRUE;
	m_DataJpeg[1].bRun = TRUE;
	InitializeCriticalSection(&m_DataJpeg[0].cs);
	InitializeCriticalSection(&m_DataJpeg[1].cs);
}

void CLowLatencyDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_0, m_cmbDevice0);
	DDX_Control(pDX, IDC_COMBO_1, m_cmbDevice1);
	DDX_Text(pDX, IDC_STATIC_SIGNAL_0, m_strSignal0);
	DDX_Text(pDX, IDC_STATIC_RESOLUTION_0, m_strResolution0);
	DDX_Text(pDX, IDC_STATIC_SCAN_SIZE_0, m_strScanSize0);
	DDX_Text(pDX, IDC_STATIC_OFFSET_0, m_strOffset0);
	DDX_Text(pDX, IDC_STATIC_FORMAT_0, m_strFormat0);
	DDX_Text(pDX, IDC_STATIC_CAPTURE_LATENCY_0, m_strCaptureLatency0);
	DDX_Text(pDX, IDC_STATIC_TOTAL_0, m_strTotalLatency0);
	DDX_Text(pDX, IDC_STATIC_SIGNAL_1, m_strSignal1);
	DDX_Text(pDX, IDC_STATIC_RESOLUTION_1, m_strResolution1);
	DDX_Text(pDX, IDC_STATIC_SCAN_SIZE_1, m_strScanSize1);
	DDX_Text(pDX, IDC_STATIC_OFFSET_1, m_strOffset1);
	DDX_Text(pDX, IDC_STATIC_FORMAT_1, m_strFormat1);
	DDX_Text(pDX, IDC_STATIC_CAPTURE_LATENCY_1, m_strCaptureLatency1);
	DDX_Text(pDX, IDC_STATIC_TOTAL_1, m_strTotalLatency1);
	DDX_Text(pDX, IDC_STATIC_HANDLE_LATENCY_0, m_strHandleLatency0);
	DDX_Text(pDX, IDC_STATIC_HANDLE_LATENCY_1, m_strHandleLatency1);
	//DDX_Text(pDX, IDC_CURRENT_FPS_0, m_strCurrentFps0);
	//DDX_Text(pDX, IDC_CURRENT_FPS_1, m_strCurrentFps1);
}

BEGIN_MESSAGE_MAP(CLowLatencyDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_WM_TIMER()
ON_WM_DESTROY()
ON_CBN_SELCHANGE(IDC_COMBO_0, &CLowLatencyDlg::OnCbnSelchangeCombo0)
ON_CBN_SELCHANGE(IDC_COMBO_1, &CLowLatencyDlg::OnCbnSelchangeCombo1)
ON_WM_MOVE()
END_MESSAGE_MAP()

// CLowLatencyDlg message handlers

BOOL CLowLatencyDlg::OnInitDialog()
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

	// create renderer
	CRect rcPos;
	CWnd *pWnd;

	pWnd = GetDlgItem(IDC_STATIC_0);
	pWnd->GetWindowRect(rcPos);
	ScreenToClient(rcPos);
	pWnd->ShowWindow(SW_HIDE);
	m_data[MODE_NORMAL].wnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcPos,
				       this, ID_VIDEO_WND);

	pWnd = GetDlgItem(IDC_STATIC_1);
	pWnd->GetWindowRect(rcPos);
	ScreenToClient(rcPos);
	pWnd->ShowWindow(SW_HIDE);
	m_data[MODE_LOW_LATENCY].wnd.Create(NULL, NULL, WS_CHILD | WS_VISIBLE,
					    rcPos, this, ID_VIDEO_WND + 1);

	// insert device
	int nItem;
	nItem = m_cmbDevice0.InsertString(0, _T("(No select channel)"));
	m_cmbDevice0.SetItemData(nItem, -1);

	nItem = m_cmbDevice1.InsertString(0, _T("(No select channel)"));
	m_cmbDevice1.SetItemData(nItem, -1);

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
			int nItem =
				m_cmbDevice0.InsertString(i + 1, szChannelName);
			m_cmbDevice0.SetItemData(nItem, (DWORD_PTR)i);

			nItem = m_cmbDevice1.InsertString(i + 1, szChannelName);
			m_cmbDevice1.SetItemData(nItem, (DWORD_PTR)i);
		}
	}
	m_cmbDevice0.SetCurSel(1);
	OpenChannel(MODE_NORMAL, 0);
	m_data[MODE_NORMAL].nSel = 1;

	if (g_nValidChannelCount > 1) {
		m_cmbDevice1.SetCurSel(2);
		OpenChannel(MODE_LOW_LATENCY, 1);
		m_data[MODE_LOW_LATENCY].nSel = 2;
	} else {
		m_cmbDevice1.SetCurSel(0);
		m_data[MODE_LOW_LATENCY].nSel = 0;
	}

	SetTimer(1, 500, NULL);

	return TRUE; // return TRUE  unless you set the focus to a control
}

void CLowLatencyDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLowLatencyDlg::OnPaint()
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
HCURSOR CLowLatencyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLowLatencyDlg::OnDestroy()
{
	CloseChannel(MODE_NORMAL);
	CloseChannel(MODE_LOW_LATENCY);

	DeleteCriticalSection(&m_DataJpeg[0].cs);
	DeleteCriticalSection(&m_DataJpeg[1].cs);
	CDialogEx::OnDestroy();
}

BOOL CLowLatencyDlg::OpenChannel(CAPTURE_MODE mode, int nIndex)
{
	CloseChannel(mode);

	DWORD cbStride = FOURCC_CalcMinStride(CAPTURE_COLOR, CAPTURE_WIDTH, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(CAPTURE_COLOR, CAPTURE_WIDTH,
						 CAPTURE_HEIGHT, cbStride);
	if (mode == MODE_NORMAL) {
		for (int i = 0; i < 5; i++) {
			m_DataNormal[i].pBuffer = new BYTE[dwFrameSize];
			memset(m_DataNormal[i].pBuffer, 0,
			       sizeof(BYTE) * dwFrameSize);
		}
	} else {
		for (int i = 0; i < 30; i++) {
			m_DataLowlatency[i].pBuffer = new BYTE[dwFrameSize];
			memset(m_DataLowlatency[i].pBuffer, 0,
			       sizeof(BYTE) * dwFrameSize);
		}
	}
	CAPTURE_DATA &data = m_data[mode];
	do {
		data.dwCapFrameCount = 0;
		data.dwHandleFrameCount = 0;
		data.llCaptureDuration = 0LL;
		data.llTotalDuration = 0LL;
		data.llGpegHandleDuration = 0LL;
		data.llTotalTime = 0LL;

		MWCAP_CHANNEL_INFO videoInfo = {0};
		if (MW_SUCCEEDED !=
		    MWGetChannelInfoByIndex(g_nValidChannel[nIndex],
					    &videoInfo)) {
			break;
		}

		// Open channel
		TCHAR szDevicePath[MAX_PATH];
		if (MW_SUCCEEDED !=
		    MWGetDevicePath(g_nValidChannel[nIndex], szDevicePath)) {
			OutputDebugStringA("ERROR: MWGetDevicePath fail !\n");
			break;
		}
		data.hChannel = MWOpenChannelByPath(szDevicePath);
		if (data.hChannel == NULL) {
			break;
		}

		m_data[mode].wnd.OpenChannel(nIndex);

		data.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		if (mode == MODE_NORMAL) {
			m_DataJpeg[mode].hThread = CreateThread(
				NULL, 0, OnNormalJpegCompressThread,
				(LPVOID)this, 0, NULL);
			if (m_DataJpeg[mode].hThread == NULL)
				break;
			data.hThread = CreateThread(NULL, 0, ThreadProc,
						    (LPVOID)this, 0, NULL);
			m_DataJpeg[mode].bRun = TRUE;
		} else {
			m_DataJpeg[mode].hThread = CreateThread(
				NULL, 0, OnLowlatencyJpegCompressThread,
				(LPVOID)this, 0, NULL);
			if (m_DataJpeg[mode].hThread == NULL)
				break;
			data.hThread = CreateThread(NULL, 0, ThreadProc1,
						    (LPVOID)this, 0, NULL);
			m_DataJpeg[mode].bRun = TRUE;
		}
		if (data.hThread == NULL) {
			break;
		}
	} while (FALSE);

	return TRUE;
}

void CLowLatencyDlg::CloseChannel(CAPTURE_MODE mode)
{
	CAPTURE_DATA &data = m_data[mode];
	if (data.hEvent != NULL && data.hThread != NULL) {
		SetEvent(data.hEvent);
		WaitForSingleObject(data.hThread, INFINITE);

		CloseHandle(data.hThread);
		data.hThread = NULL;

		CloseHandle(data.hEvent);
		data.hEvent = NULL;
	}

	if (mode == MODE_LOW_LATENCY) {
		if (m_DataJpeg[mode].bRun == TRUE &&
		    m_DataJpeg[mode].hThread != NULL) {
			m_DataJpeg[mode].bRun = FALSE;
			WaitForSingleObject(m_DataJpeg[mode].hThread, INFINITE);
			CloseHandle(m_DataJpeg[mode].hThread);
			m_DataJpeg[mode].hThread = NULL;
		}
		for (int i = 0; i < 30; i++) {
			if (m_DataLowlatency[i].pBuffer != NULL) {
				delete m_DataLowlatency[i].pBuffer;
				m_DataLowlatency[i].pBuffer = NULL;
			}
		}
	} else {
		if (m_DataJpeg[mode].bRun == TRUE &&
		    m_DataJpeg[mode].hThread != NULL) {
			m_DataJpeg[mode].bRun = FALSE;
			WaitForSingleObject(m_DataJpeg[mode].hThread, INFINITE);
			CloseHandle(m_DataJpeg[mode].hThread);
			m_DataJpeg[mode].hThread = NULL;
		}
		for (int i = 0; i < 5; i++) {
			if (m_DataNormal[i].pBuffer != NULL) {
				delete m_DataNormal[i].pBuffer;
				m_DataNormal[i].pBuffer = NULL;
			}
		}
	}
	if (data.hChannel != NULL) {
		MWCloseChannel(data.hChannel);
		data.hChannel = NULL;
	}
	m_data[mode].wnd.CloseChannel();
}

DWORD CLowLatencyDlg::ThreadProc(CAPTURE_MODE mode)
{
	CAPTURE_DATA &data = m_data[mode];
	SetPriorityClass(data.hThread, REALTIME_PRIORITY_CLASS);

	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD cbStride = FOURCC_CalcMinStride(CAPTURE_COLOR, CAPTURE_WIDTH, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(CAPTURE_COLOR, CAPTURE_WIDTH,
						 CAPTURE_HEIGHT, cbStride);
	BYTE *byBuffer = new BYTE[dwFrameSize];
	memset(byBuffer, 0, sizeof(char) * dwFrameSize);

	MW_RESULT xr;
	do {
		xr = MWPinVideoBuffer(data.hChannel, byBuffer, dwFrameSize);
		if (xr != MW_SUCCEEDED)
			break;

		xr = MWStartVideoCapture(data.hChannel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
		if (MW_SUCCEEDED !=
		    MWGetVideoBufferInfo(data.hChannel, &videoBufferInfo))
			break;

		MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
		xr = MWGetVideoFrameInfo(
			data.hChannel, videoBufferInfo.iNewestBufferedFullFrame,
			&videoFrameInfo);
		if (xr != MW_SUCCEEDED)
			break;

		HNOTIFY hNotify = MWRegisterNotify(
			data.hChannel, hNotifyEvent,
			(mode == MODE_NORMAL
				 ? MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED
				 : MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING) |
				MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE |
				MWCAP_NOTIFY_VIDEO_INPUT_SOURCE_CHANGE);
		if (hNotify == NULL)
			break;

		LONGLONG llCapBegin = 0LL;
		xr = MWGetDeviceTime(data.hChannel, &llCapBegin);
		if (xr != MW_SUCCEEDED)
			break;

		xr = MWGetVideoSignalStatus(data.hChannel, &data.status);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		int num = 0;
		LONGLONG llCapEnd = llCapBegin;
		LONGLONG llLastEnd = llCapBegin;
		m_DataJpeg[mode].nCount = 0;
		while (TRUE) {
			HANDLE aEvent[] = {data.hEvent, hNotifyEvent};
			DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE,
							     INFINITE);
			if (dwRet == WAIT_OBJECT_0)
				break;

			xr = MWGetVideoSignalStatus(data.hChannel,
						    &data.status);
			if (xr != MW_SUCCEEDED)
				break;

			ULONGLONG ullStatusBits = 0;
			xr = MWGetNotifyStatus(data.hChannel, hNotify,
					       &ullStatusBits);
			if (xr != MW_SUCCEEDED)
				continue;

			xr = MWGetVideoBufferInfo(data.hChannel,
						  &videoBufferInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			if (ullStatusBits & MWCAP_NOTIFY_VIDEO_FRAME_BUFFERED) {
				xr = MWGetVideoFrameInfo(
					data.hChannel,
					videoBufferInfo.iNewestBuffered,
					&videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				if (m_DataJpeg[mode].nCount < 4) {
					xr = MWCaptureVideoFrameToVirtualAddress(
						data.hChannel,
						videoBufferInfo.iNewestBuffered,
						byBuffer, dwFrameSize, cbStride,
						FALSE, NULL, CAPTURE_COLOR,
						CAPTURE_WIDTH, CAPTURE_HEIGHT);
					do {
						WaitForSingleObject(
							hCaptureEvent,
							INFINITE);
						xr = MWGetVideoCaptureStatus(
							data.hChannel,
							&captureStatus);
					} while (
						xr == MW_SUCCEEDED &&
						!captureStatus.bFrameCompleted);

					MWGetDeviceTime(data.hChannel,
							&llCapEnd);
					m_DataNormal[num].llCapEnd = llCapEnd;

					memcpy(m_DataNormal[num].pBuffer,
					       byBuffer, dwFrameSize);

					num = (num > 4) ? 0 : num++;

					EnterCriticalSection(
						&m_DataJpeg[mode].cs);
					m_queNormal.push(m_DataNormal[num]);
					m_DataJpeg[mode].nCount++;
					LeaveCriticalSection(
						&m_DataJpeg[mode].cs);

					LONGLONG llCurrent = 0LL;
					xr = MWGetDeviceTime(data.hChannel,
							     &llCurrent);
					if (xr != MW_SUCCEEDED)
						continue;

					data.dwCapFrameCount++;

					data.llTotalDuration +=
						(llCurrent -
						 videoFrameInfo
							 .allFieldStartTimes[0]);
					if (data.status.bInterlaced)
						data.llCaptureDuration +=
							(llCapEnd -
							 videoFrameInfo
								 .allFieldBufferedTimes
									 [1]);
					else
						data.llCaptureDuration +=
							(llCapEnd -
							 videoFrameInfo
								 .allFieldBufferedTimes
									 [0]);
				}
				if (data.dwCapFrameCount % 300 == 0) {
					data.dwCapFrameCount = 0;
					data.llGpegHandleDuration = 0;
					data.llCaptureDuration = 0LL;
					data.llTotalDuration = 0LL;
					data.llTotalTime = 0;
				}
			} else if (ullStatusBits &
				   MWCAP_NOTIFY_VIDEO_FRAME_BUFFERING) {
				xr = MWGetVideoFrameInfo(
					data.hChannel,
					videoBufferInfo.iNewestBuffering,
					&videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				if (m_DataJpeg[mode].nCount < 2) {
					m_DataLowlatency[num].bStart = TRUE;
					xr = MWCaptureVideoFrameToVirtualAddressEx(
						data.hChannel,
						videoBufferInfo.iNewestBuffering,
						byBuffer, dwFrameSize, cbStride,
						FALSE, NULL, CAPTURE_COLOR,
						CAPTURE_WIDTH, CAPTURE_HEIGHT,
						0, CAPTURE_PART_LINE, NULL,
						NULL, 0, 100, 0, 100, 0,
						MWCAP_VIDEO_DEINTERLACE_BLEND,
						MWCAP_VIDEO_ASPECT_RATIO_IGNORE,
						NULL, NULL, 0, 0,
						MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
						MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
						MWCAP_VIDEO_SATURATION_UNKNOWN);

					do {
						WaitForSingleObject(
							hCaptureEvent,
							INFINITE);
						xr = MWGetVideoCaptureStatus(
							data.hChannel,
							&captureStatus);

						memcpy(m_DataLowlatency[num]
							       .pBuffer,
						       &byBuffer[captureStatus
									 .cyCompletedPrev *
								 cbStride],
						       (captureStatus
								.cyCompleted -
							captureStatus
								.cyCompletedPrev) *
							       cbStride);
						m_DataLowlatency[num]
							.bCompleted =
							captureStatus
								.bFrameCompleted;
						m_DataLowlatency[num]
							.nCyCompleted =
							captureStatus
								.cyCompleted;

						if (captureStatus
							    .bFrameCompleted) {
							MWGetDeviceTime(
								data.hChannel,
								&llCapEnd);
							m_DataLowlatency[num]
								.llCapEnd =
								llCapEnd;
							data.dwCapFrameCount++;
						}

						EnterCriticalSection(
							&m_DataJpeg[mode].cs);
						m_queLowlatency.push(
							m_DataLowlatency[num]);
						LeaveCriticalSection(
							&m_DataJpeg[mode].cs);

						num = ((++num) > 29) ? 0 : num;
						m_DataLowlatency[num].bStart =
							FALSE;
					} while (
						xr == MW_SUCCEEDED &&
						!captureStatus.bFrameCompleted);

					EnterCriticalSection(
						&m_DataJpeg[mode].cs);
					m_DataJpeg[mode].nCount++;
					LeaveCriticalSection(
						&m_DataJpeg[mode].cs);

					xr = MWGetVideoFrameInfo(
						data.hChannel,
						videoBufferInfo.iNewestBuffering,
						&videoFrameInfo);
					if (xr != MW_SUCCEEDED)
						continue;

					LONGLONG llCurrent = 0LL;
					xr = MWGetDeviceTime(data.hChannel,
							     &llCurrent);
					if (xr != MW_SUCCEEDED)
						continue;

					data.llTotalDuration +=
						(llCurrent -
						 videoFrameInfo
							 .allFieldStartTimes[0]);

					if (data.status.bInterlaced)
						data.llCaptureDuration +=
							(llCapEnd -
							 videoFrameInfo
								 .allFieldBufferedTimes
									 [1]);
					else
						data.llCaptureDuration +=
							(llCapEnd -
							 videoFrameInfo
								 .allFieldBufferedTimes
									 [0]);
				}

				if (data.dwCapFrameCount % 300 == 0) {
					data.dwCapFrameCount = 0;
					data.llCaptureDuration = 0LL;
					data.llTotalDuration = 0LL;
				}
			}

			if ((ullStatusBits &
			     (MWCAP_NOTIFY_VIDEO_SIGNAL_CHANGE |
			      MWCAP_NOTIFY_VIDEO_INPUT_SOURCE_CHANGE)) ||
			    (videoFrameInfo.allFieldStartTimes[0] == -1) ||
			    (data.status.bInterlaced &&
			     (videoFrameInfo.allFieldStartTimes[1] == -1))) {
				data.dwCapFrameCount = 0LL;
				data.dwHandleFrameCount = 0LL;
				data.llCaptureDuration = 0LL;
				data.llTotalDuration = 0LL;
				data.llGpegHandleDuration = 0LL;
				data.llTotalTime = 0LL;
				continue;
			}

			//	xr = MWGetVideoCaptureStatus(data.hChannel, &captureStatus);
		}

		xr = MWUnregisterNotify(data.hChannel, hNotify);

		xr = MWStopVideoCapture(data.hChannel);

		xr = MWUnpinVideoBuffer(data.hChannel, byBuffer);

	} while (FALSE);

	delete[] byBuffer;
	byBuffer = NULL;

	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);
	hCaptureEvent = NULL;
	return 0;
}
DWORD CLowLatencyDlg::OnNormalJpegCompressThreadProc(CAPTURE_MODE mode)
{
	CAPTURE_DATA &data = m_data[mode];
	LONGLONG llLastEnd = 0LL;
	MWGetDeviceTime(data.hChannel, &llLastEnd);
	while (m_DataJpeg[mode].bRun) {
		while (m_queNormal.empty() == FALSE) {
			LONGLONG llTimeBegin = 0;
			LONGLONG llCurrent = 0;
			MWGetDeviceTime(data.hChannel, &llTimeBegin);

			EnterCriticalSection(&m_DataJpeg[mode].cs);
			COMPRESS_DATA partData = m_queNormal.front();
			LeaveCriticalSection(&m_DataJpeg[mode].cs);

			InitJPEGfile(partData.pBuffer);

			MWGetDeviceTime(data.hChannel, &llCurrent);

			data.llGpegHandleDuration +=
				(llCurrent - partData.llCapEnd);
			data.llTotalTime += llCurrent - llLastEnd;
			llLastEnd = llCurrent;
			data.dwHandleFrameCount++;

			EnterCriticalSection(&m_DataJpeg[mode].cs);
			m_queNormal.pop();
			m_DataJpeg[mode].nCount--;
			LeaveCriticalSection(&m_DataJpeg[mode].cs);

			if (data.dwHandleFrameCount % 300 == 0) {
				data.dwHandleFrameCount = 0;
				data.llGpegHandleDuration = 0LL;
				data.llTotalTime = 0LL;
			}
		}
	}
	return 0;
}
METHODDEF(void) jpeg_mem_error_exit(j_common_ptr cinfo)
{
	char err_msg[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo, err_msg);
}

METHODDEF(void) jpeg_mem_warning_message(j_common_ptr cinfo, int msg_level)
{
	char err_msg[JMSG_LENGTH_MAX];
	(*cinfo->err->format_message)(cinfo, err_msg);
}
DWORD CLowLatencyDlg::OnLowlatencyJpegCompressThreadProc(CAPTURE_MODE mode)
{
	CAPTURE_DATA &data = m_data[mode];

	jpeg_compress_struct cinfo;
	FILE *pOutFile = NULL;

	JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
	int nStride = CAPTURE_WIDTH *
		      BIT_DEPTH; /* JSAMPLEs per row in image_buffer */

	int nLastLines = 0;
	LONGLONG llCurrent = 0;
	LONGLONG llLastEnd = 0;

	MWGetDeviceTime(data.hChannel, &llLastEnd);

	while (m_DataJpeg[mode].bRun) {
		while (m_queLowlatency.empty() != TRUE) {
			//m_mutex.Lock();
			EnterCriticalSection(&m_DataJpeg[mode].cs);
			COMPRESS_DATA partData = m_queLowlatency.front();
			//m_mutex.Unlock();
			LeaveCriticalSection(&m_DataJpeg[mode].cs);
			if (partData.bStart == TRUE) {
				//jpeg_compress_struct cinfo;
				struct jpeg_error_mgr jerr;
				cinfo.err = jpeg_std_error(&jerr);

				jerr.error_exit = jpeg_mem_error_exit;
				jerr.emit_message = jpeg_mem_warning_message;
				jpeg_create_compress(&cinfo);

				if ((pOutFile = fopen("Lowlatency.jpeg",
						      "wb")) == NULL) {
					fprintf(stderr, "can't open %s\n",
						"Lowlatency.jpeg");
					AfxMessageBox(
						L"Can't open Lowlatency.jpeg.Please restart the application with administrator permission.");
					exit(1);
				}

				jpeg_stdio_dest(&cinfo, pOutFile);
				cinfo.image_width =
					CAPTURE_WIDTH; /* image width and height, in pixels */
				cinfo.image_height = CAPTURE_HEIGHT;
				cinfo.input_components =
					BIT_DEPTH; /* # of color components per pixel */
				cinfo.in_color_space =
					JCS_EXT_RGBA; /* colorspace of input image */
				//cinfo.jpeg_color_space = JCS_EXT_RGBA;
				cinfo.dct_method = JDCT_IFAST;

				jpeg_set_defaults(&cinfo);
				jpeg_set_quality(
					&cinfo, 50,
					TRUE /* limit to baseline-JPEG values */);

				jpeg_start_compress(&cinfo, TRUE);

				nLastLines = 0;
			}

			int nLineCount = partData.nCyCompleted - nLastLines;

			for (int i = 0; i < nLineCount; i++) {
				row_pointer[0] = &partData.pBuffer[i * nStride];
				jpeg_write_scanlines(&cinfo, row_pointer, 1);
			}
			nLastLines = partData.nCyCompleted;

			if (partData.bCompleted) {
				jpeg_finish_compress(&cinfo);
				fclose(pOutFile);
				jpeg_destroy_compress(&cinfo);

				MWGetDeviceTime(data.hChannel, &llCurrent);
				data.llGpegHandleDuration +=
					(llCurrent - partData.llCapEnd);

				data.llTotalTime += llCurrent - llLastEnd;
				llLastEnd = llCurrent;

				data.dwHandleFrameCount++;
				nLastLines = 0;

				EnterCriticalSection(&m_DataJpeg[mode].cs);
				m_DataJpeg[mode].nCount--;
				LeaveCriticalSection(&m_DataJpeg[mode].cs);
			}

			EnterCriticalSection(&m_DataJpeg[mode].cs);
			m_queLowlatency.pop();
			LeaveCriticalSection(&m_DataJpeg[mode].cs);

			if (data.dwHandleFrameCount % 300 == 0 &&
			    data.dwHandleFrameCount != 0) {
				data.dwHandleFrameCount = 0;
				data.llGpegHandleDuration = 0LL;
				data.llTotalTime = 0;
			}
		}
	}

	return 0;
}

void CLowLatencyDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1) {
		CAPTURE_DATA &normal = m_data[MODE_NORMAL];
		CAPTURE_DATA &lowlatency = m_data[MODE_LOW_LATENCY];

		int nSel0 = m_cmbDevice0.GetCurSel();
		if (nSel0 == 0 || nSel0 != normal.nSel ||
		    normal.status.state != MWCAP_VIDEO_SIGNAL_LOCKED) {
			m_strSignal0 = _T("-");
			m_strResolution0 = _T("-");
			m_strScanSize0 = _T("-");
			m_strOffset0 = _T("-");
			m_strFormat0 = _T("-");
			m_strCaptureLatency0 = _T("-");
			m_strTotalLatency0 = _T("-");
			m_strHandleLatency0 = _T("-");
			//m_strCurrentFps0		= _T("-");
		} else {
			m_strSignal0 = GetStatus(normal.status.state);
			m_strResolution0 = GetResolutions(
				normal.status.cx, normal.status.cy,
				normal.status.dwFrameDuration);
			m_strScanSize0 = GetScanSize(normal.status.cxTotal,
						     normal.status.cyTotal);
			m_strOffset0 =
				GetOffset(normal.status.x, normal.status.y);
			m_strFormat0 = GetFormat(normal.status.dwFrameDuration);
			if (normal.dwCapFrameCount > 0 &&
			    normal.dwHandleFrameCount > 0) {
				m_strCaptureLatency0 = GetLatency(
					(DWORD)(normal.llCaptureDuration /
						normal.dwCapFrameCount));
				m_strTotalLatency0 = GetLatency(
					(DWORD)(normal.llTotalDuration /
						normal.dwCapFrameCount) +
					(DWORD)(normal.llGpegHandleDuration /
						normal.dwHandleFrameCount));
				m_strHandleLatency0 = GetLatency(
					(DWORD)(normal.llGpegHandleDuration /
						normal.dwCapFrameCount));
			}
			//if (normal.llTotalTime > 0)
			//m_strCurrentFps0		= GetFPS((((float)normal.dwHandleFrameCount / normal.llTotalTime)));
		}
		int nSel1 = m_cmbDevice1.GetCurSel();
		if (nSel1 == 0 || nSel1 != lowlatency.nSel ||
		    lowlatency.status.state != MWCAP_VIDEO_SIGNAL_LOCKED) {
			m_strSignal1 = _T("-");
			m_strResolution1 = _T("-");
			m_strScanSize1 = _T("-");
			m_strOffset1 = _T("-");
			m_strFormat1 = _T("-");
			m_strCaptureLatency1 = _T("-");
			m_strTotalLatency1 = _T("-");
			m_strHandleLatency1 = _T("-");
			//m_strCurrentFps1		= _T("-");
		} else {
			m_strSignal1 = GetStatus(lowlatency.status.state);
			m_strResolution1 = GetResolutions(
				lowlatency.status.cx, lowlatency.status.cy,
				lowlatency.status.dwFrameDuration);
			m_strScanSize1 = GetScanSize(lowlatency.status.cxTotal,
						     lowlatency.status.cyTotal);
			m_strOffset1 = GetOffset(lowlatency.status.x,
						 lowlatency.status.y);
			m_strFormat1 =
				GetFormat(lowlatency.status.dwFrameDuration);
			if (lowlatency.dwCapFrameCount > 0 &&
			    lowlatency.dwHandleFrameCount > 0) {
				m_strCaptureLatency1 = GetLatency(
					(DWORD)(lowlatency.llCaptureDuration /
						lowlatency.dwCapFrameCount));
				m_strTotalLatency1 = GetLatency(
					(DWORD)(lowlatency.llTotalDuration /
						lowlatency.dwCapFrameCount) +
					(DWORD)(lowlatency.llGpegHandleDuration /
						lowlatency.dwHandleFrameCount));
				m_strHandleLatency1 = GetLatency((
					DWORD)(lowlatency.llGpegHandleDuration /
					       lowlatency.dwHandleFrameCount));
			}
			//if (lowlatency.llTotalTime > 0)
			//{
			//m_strCurrentFps1		= GetFPS(((float)lowlatency.dwHandleFrameCount / lowlatency.llTotalTime));
			//}
		}

		UpdateData(FALSE);
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CLowLatencyDlg::OnCbnSelchangeCombo0()
{
	int nSel = m_cmbDevice0.GetCurSel();
	if (nSel == -1)
		return;
	if (nSel == m_data[MODE_NORMAL].nSel ||
	    ((nSel == m_data[MODE_LOW_LATENCY].nSel) && nSel != 0)) {
		m_cmbDevice0.SetCurSel(m_data[MODE_NORMAL].nSel);
		return;
	}

	int nIndex = (int)m_cmbDevice0.GetItemData(nSel);
	CloseChannel(MODE_NORMAL);

	if (nIndex != -1) {
		OpenChannel(MODE_NORMAL, nIndex);
	}
	m_data[MODE_NORMAL].nSel = nSel;
}

void CLowLatencyDlg::OnCbnSelchangeCombo1()
{
	int nSel = m_cmbDevice1.GetCurSel();
	if (nSel == -1)
		return;
	if (nSel == m_data[MODE_LOW_LATENCY].nSel ||
	    ((nSel == m_data[MODE_NORMAL].nSel) && nSel != 0)) {
		m_cmbDevice1.SetCurSel(m_data[MODE_LOW_LATENCY].nSel);
		return;
	}

	int nIndex = (int)m_cmbDevice1.GetItemData(nSel);
	CloseChannel(MODE_LOW_LATENCY);

	if (nIndex != -1) {
		OpenChannel(MODE_LOW_LATENCY, nIndex);
	}
	m_data[MODE_LOW_LATENCY].nSel = nSel;
}

void CLowLatencyDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);
}

BOOL CLowLatencyDlg::PreTranslateMessage(MSG *pMsg)
{
	if (pMsg->message == WM_KEYDOWN &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN) &&
	    pMsg->wParam)
		return TRUE;

	return CDialogEx::PreTranslateMessage(pMsg);
}
