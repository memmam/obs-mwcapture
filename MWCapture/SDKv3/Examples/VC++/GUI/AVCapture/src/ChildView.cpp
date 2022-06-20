/************************************************************************************************/
// ChildView.cpp : implementation of the CChildView class

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
#include "AVCapture.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080

#define PREVIEW_DURATION 400000 //166667//166667//666667

// MWFOURCC_GREY
// MWFOURCC_Y800
// MWFOURCC_Y8
// MWFOURCC_Y16
// MWFOURCC_RGB16
// MWFOURCC_BGR16
// MWFOURCC_YUY2
// MWFOURCC_YUYV
// MWFOURCC_BGRA
// MWFOURCC_RGBA
// MWFOURCC_ARGB
// MWFOURCC_ABGR
// MWFOURCC_BGR24
// MWFOURCC_RGB24
// MWFOURCC_UYVY
// MWFOURCC_NV12
// MWFOURCC_NV21
// MWFOURCC_NV16
// MWFOURCC_NV61
// MWFOURCC_YVYU
// MWFOURCC_VYUY
// MWFOURCC_I422
// MWFOURCC_YV16
// MWFOURCC_I420
// MWFOURCC_IYUV
// MWFOURCC_YV12
// MWFOURCC_P010
// MWFOURCC_P210
// MWFOURCC_IYU2
// MWFOURCC_V308
// MWFOURCC_AYUV
// MWFOURCC_UYVA
// MWFOURCC_V408
// MWFOURCC_VYUA
// MWFOURCC_Y410
// MWFOURCC_RGB10
// MWFOURCC_BGR10
#define PREVIEW_FOURCC MWFOURCC_NV12

#define WM_VIDEOSIGNAL_CHANGED (WM_USER + 301)
#define MW_VIDEORENDER_RESET (WM_USER + 302)
#define WM_DISK_SIZE_OVER (WM_USER + 303)
// CChildView

CChildView::CChildView()
{
	m_hChannel = NULL;
	m_p_VideoRenderThread = NULL;
	m_pAudioRenderer = NULL;

	m_hPowerNotify = NULL;

	m_nChannel = -1;

	m_nClip = CLIP_NONE;

	m_bRecord = FALSE;
	m_bufWidth = 0;
	m_bufHeight = 0;

	memset(m_cs_dir, 0, 256);
	memset(m_wcs_dir, 0, sizeof(WCHAR) * 256);

	m_dwCaptureCount = 0;
	m_hExitEncodeThread = NULL;
	m_hEncodeThread = NULL;
	m_hEncodeFrame = NULL;

	m_p_VideoRenderThread = new CMWDXRenderThread();
	m_p_RenderBuffer = new CMWRenderBuffer();
	m_p_RenderBuffer->alloc_buf(4120 * 2160 * 2);
	m_previewThread.SetRenderBuffer(m_p_RenderBuffer);
	m_p_VideoRenderThread->set_render_buffer(m_p_RenderBuffer);

	m_u32Record = 0;

	InitializeCriticalSection(&m_cs);
}

CChildView::~CChildView()
{
	if (m_p_VideoRenderThread) {
		delete m_p_VideoRenderThread;
		m_p_VideoRenderThread = NULL;
	}

	if (m_p_RenderBuffer) {
		delete m_p_RenderBuffer;
		m_p_RenderBuffer = NULL;
	}

	DeleteCriticalSection(&m_cs);
}

void CChildView::OnFileRecordItem(UINT nID)
{
	uint32_t t_u32_id = nID - ID_STARTRECORD_CPU;
	;

	// TODO: Add your command handler code here
	CString strFilter;
	strFilter = _T("mp4 Files (*.mp4)|*.mp4||");

	CFileDialog dlg(FALSE, _T("a"), NULL, 4 | 2, strFilter, this);
	if (dlg.DoModal() != IDOK)
		return;

	CString strFileRecord = dlg.GetPathName();
	CString strFileFolder = dlg.GetFolderPath();

	int nFind = strFileRecord.ReverseFind(L'.');
	if (nFind == -1)
		return;

	CString strExtFilePath =
		strFileRecord.Right(strFileRecord.GetLength() - nFind - 1);
	if (strExtFilePath.CompareNoCase(_T("mp4")) == 0) {
		// check disk space
		WCHAR *wpfolder = (WCHAR *)strFileFolder.GetBuffer(
			strFileFolder.GetLength());
		int len = WideCharToMultiByte(CP_ACP, 0, wpfolder,
					      wcslen(wpfolder), NULL, 0, NULL,
					      NULL);
		memset(m_cs_dir, 0, 256);
		memset(m_wcs_dir, 0, 256 * sizeof(WCHAR));
		WideCharToMultiByte(CP_ACP, 0, wpfolder, wcslen(wpfolder),
				    m_cs_dir, len, NULL, NULL);
		swprintf_s(m_wcs_dir, _T("%s"), wpfolder);

		mw_disk_size_t t_disk_size;
		BOOL t_ret = get_disk_size(t_disk_size);
		if (t_ret) {
			if (t_disk_size.m_u32_gb < 1 &&
			    t_disk_size.m_u32_mb < 200) {
				TCHAR szLog[512] = {0};
				swprintf_s(
					szLog,
					_T("'%s' current free disk size is not enough."),
					m_wcs_dir);
				AfxMessageBox(szLog);
				return;
			}
		}

		MWCAP_VIDEO_COLOR_FORMAT colorFormat;
		MWCAP_VIDEO_QUANTIZATION_RANGE quantRange;
		m_previewThread.GetCaptureVideoFormat(m_bufWidth, m_bufHeight,
						      colorFormat, quantRange);

		m_n_id = 0;
		WCHAR *wpFileName = (WCHAR *)strFileRecord.GetBuffer(
			strFileRecord.GetLength());

		len = WideCharToMultiByte(CP_ACP, 0, wpFileName,
					  wcslen(wpFileName), NULL, 0, NULL,
					  NULL);
		char *pFileName = new char[len + 1];
		WideCharToMultiByte(CP_ACP, 0, wpFileName, wcslen(wpFileName),
				    pFileName, len, NULL, NULL);
		pFileName[len] = '\0';

		BOOL bLPCM = FALSE;
		BYTE cBitsPerSample = 0;
		DWORD dwSampleRate = 0;
		m_previewThread.GetInputAudioFormat(bLPCM, cBitsPerSample,
						    dwSampleRate);

		BOOL t_b_init = FALSE;
		t_b_init = m_FileRecord.InitW(wpFileName, m_bufWidth,
					      m_bufHeight,
					      PREVIEW_DURATION / 10000,
					      dwSampleRate, 2, t_u32_id);
		strFileRecord.ReleaseBuffer();
		delete pFileName;
		pFileName = NULL;
		if (!t_b_init) {
			TCHAR szLog[MAX_PATH];
			if (t_u32_id == 0)
				swprintf_s(
					szLog,
					_T("Start record fail with ffmpeg !\n"));
			else
				swprintf_s(szLog,
					   _T("Start record fail with %S!\n"),
					   m_FileRecord.GetGPUInfo()
						   .at(t_u32_id - 1)
						   .gpu_name);
			AfxMessageBox(szLog);
			return;
		}

		m_hEncodeFrame = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hExitEncodeThread = CreateEvent(NULL, FALSE, FALSE, NULL);
		m_hEncodeThread = CreateThread(NULL, 0, EncodeVideoFrameThread,
					       (LPVOID)this, 0, NULL);

		m_bRecord = TRUE;

		m_u32Record = t_u32_id;
	} else {
		CString strFileName = dlg.GetFileName();
		TCHAR *ptcFileName = strFileName.GetBuffer();
		TCHAR szLog[MAX_PATH];
		swprintf_s(
			szLog,
			_T("Start record fail because of invalid file name '%s' !\nWe only support file name with case-insensitive suffix of 'mp4' !"),
			ptcFileName);
		strFileName.ReleaseBuffer();
		AfxMessageBox(szLog);
		return;
	}
}

void CChildView::OnUpdateFileRecordItem(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID == ID_STARTRECORD_CPU + m_u32Record)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);
	pCmdUI->Enable(!m_bRecord);
}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()

ON_COMMAND_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16, OnDeviceItem)
ON_UPDATE_COMMAND_UI_RANGE(ID_DEVICE_BEGIN, ID_DEVICE_BEGIN + 16,
			   OnUpdateDeviceItem)

ON_COMMAND_RANGE(ID_STARTRECORD_CPU, ID_STARTRECORD_CPU + 16, OnFileRecordItem)
ON_UPDATE_COMMAND_UI_RANGE(ID_STARTRECORD_CPU, ID_STARTRECORD_CPU + 16,
			   OnUpdateFileRecordItem)

ON_WM_ERASEBKGND()
ON_MESSAGE(WM_VIDEOSIGNAL_CHANGED, &CChildView::OnMsgVideoSignalChanged)
ON_MESSAGE(MW_VIDEORENDER_RESET, &CChildView::OnMsgVideoRenderReset)
ON_MESSAGE(WM_DISK_SIZE_OVER, &CChildView::OnMsgDiskSizeOver)
//ON_WM_POWERBROADCAST()
ON_WM_CLOSE()
ON_COMMAND(IDM_FILE_NOCLIP, &CChildView::OnFileNoclip)
ON_COMMAND(IDM_CLIP_TOPLEFT, &CChildView::OnClipTopleft)
ON_COMMAND(IDM_CLIP_TOPRIGHT, &CChildView::OnClipTopright)
ON_UPDATE_COMMAND_UI(IDM_FILE_NOCLIP, &CChildView::OnUpdateFileNoclip)
ON_UPDATE_COMMAND_UI(IDM_CLIP_TOPLEFT, &CChildView::OnUpdateClipTopleft)
ON_UPDATE_COMMAND_UI(IDM_CLIP_TOPRIGHT, &CChildView::OnUpdateClipTopright)
ON_COMMAND(IDM_CLIP_BOTTOMLEFT, &CChildView::OnClipBottomleft)
ON_UPDATE_COMMAND_UI(IDM_CLIP_BOTTOMLEFT, &CChildView::OnUpdateClipBottomleft)
ON_COMMAND(IDM_CLIP_BOTTOMRIGHT, &CChildView::OnClipBottomright)
ON_UPDATE_COMMAND_UI(IDM_CLIP_BOTTOMRIGHT, &CChildView::OnUpdateClipBottomright)
ON_COMMAND(ID_FILE_STOPRECORD, &CChildView::OnFileStoprecord)
ON_UPDATE_COMMAND_UI(ID_FILE_STOPRECORD, &CChildView::OnUpdateFileStoprecord)
END_MESSAGE_MAP()

// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT &cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW),
		reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}

DWORD CChildView::EncodeVideoFrameThreadProc()
{
	SetThreadPriority(m_hEncodeThread, THREAD_PRIORITY_TIME_CRITICAL);

	HANDLE aEvents[2] = {m_hExitEncodeThread, m_hEncodeFrame};

	clock_t t_begin = clock();

	m_p_RenderBuffer->reset_encode_id();

	uint32_t t_u32_id = -1;
	uint32_t t_u32_id_encoder = -1;

	while (TRUE) {
		DWORD dwRet =
			WaitForMultipleObjects(2, aEvents, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0)
			break;
		else if (dwRet == WAIT_OBJECT_0 + 1) {
			if (m_bRecord == TRUE) {
				mw_buffer_t *t_p_buf =
					m_p_RenderBuffer->get_buf_encode(
						t_u32_id);
				if (t_u32_id == t_u32_id_encoder)
					continue;
				else {
					t_u32_id_encoder = t_u32_id;
					if (t_p_buf != NULL) {
						m_FileRecord.WriteVideoFrame(
							(unsigned char *)t_p_buf
								->m_p_buf,
							t_p_buf->m_u64_timestamp);
					}
				}

				clock_t t_now = clock();
				if (t_now - t_begin > 30 * CLOCKS_PER_SEC) {
					t_begin = t_now;
					PostMessage(WM_DISK_SIZE_OVER);
				}
			}
		}
	}

	return 0;
}

BOOL CChildView::get_disk_size(mw_disk_size_t &t_size)
{
	BOOL t_b_ret = false;

	ULARGE_INTEGER t_free_bytes_available_to_caller = {0};
	ULARGE_INTEGER t_total_number_of_bytes = {0};
	ULARGE_INTEGER t_total_number_of_free_bytes = {0};

	t_b_ret = GetDiskFreeSpaceExA(m_cs_dir,
				      &t_free_bytes_available_to_caller,
				      &t_total_number_of_bytes,
				      &t_total_number_of_free_bytes);
	if (t_b_ret) {
		uint64_t t_u64_freebytes_avaiable_to_user =
			t_free_bytes_available_to_caller.QuadPart;
		uint64_t t_u64_total_number_bytes =
			t_total_number_of_bytes.QuadPart;
		uint64_t t_u64_total_number_of_free_bytes =
			t_total_number_of_free_bytes.QuadPart;

		t_size.m_u32_b = t_u64_freebytes_avaiable_to_user & 1023;
		t_size.m_u32_kb = (t_u64_freebytes_avaiable_to_user >> 10) &
				  1023;
		t_size.m_u32_mb = (t_u64_freebytes_avaiable_to_user >> 20) &
				  1023;
		t_size.m_u32_gb = (t_u64_freebytes_avaiable_to_user >> 30);
	}

	return t_b_ret;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Do not call CWnd::OnPaint() for painting messages
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nChannel = 0;
	OpenPreview(m_nChannel);

	GUID gNotify = GUID_MONITOR_POWER_ON;
	m_hPowerNotify = RegisterPowerSettingNotification(
		GetSafeHwnd(), &gNotify, DEVICE_NOTIFY_WINDOW_HANDLE);

	return 0;
}

void CChildView::OnDestroy()
{
	if (m_bRecord == TRUE)
		OnFileStoprecord();

	ClosePreview();

	if (m_hPowerNotify != NULL) {
		UnregisterPowerSettingNotification(m_hPowerNotify);
		m_hPowerNotify = NULL;
	}

	CWnd::OnDestroy();
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

void CChildView::OnVideoSignalChanged(int cx, int cy, DWORD dwParam)
{
	PostMessage(WM_VIDEOSIGNAL_CHANGED);
}

void CChildView::OnCaptureCallback(const BYTE *pbyData, int cbStride,
				   DWORD dwParam)
{
	CMWAutoLock lock(m_lockVideo);

	if (m_p_VideoRenderThread != NULL)
		m_p_VideoRenderThread->render();

	if (m_bRecord == TRUE) {
		SetEvent(m_hEncodeFrame);
	}
}

void CChildView::OnAudioSignalChanged(BOOL bChannelValid, BOOL bPCM,
				      BYTE cBitsPerSample, DWORD dwSampleRate,
				      DWORD dwParam)
{
	CMWAutoLock lock(m_lockAudio);

	PostMessage(WM_VIDEOSIGNAL_CHANGED);
}

void CChildView::OnAudioCallback(const BYTE *pbyData, int cbSize,
				 uint64_t u64TimeStamp, DWORD dwParam)
{
	CMWAutoLock lock(m_lockAudio);

	if (m_pAudioRenderer) {
		m_pAudioRenderer->PutFrame(pbyData, cbSize);
	}

	if (m_bRecord == TRUE)
		m_FileRecord.WriteAudioframe((BYTE *)pbyData, cbSize,
					     u64TimeStamp);
}

vector<mw_venc_gpu_info_t> CChildView::GetGPUInfos()
{
	return m_FileRecord.GetGPUInfo();
}

void CChildView::OnDeviceItem(UINT nID)
{
	ClosePreview();

	int nChannel = nID - ID_DEVICE_BEGIN - 1;
	OpenPreview(nChannel);
}

void CChildView::OnUpdateDeviceItem(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID == ID_DEVICE_BEGIN + 1 + m_nChannel)
		pCmdUI->SetCheck(TRUE);
	else
		pCmdUI->SetCheck(FALSE);

	pCmdUI->Enable(!m_bRecord);
}

BOOL CChildView::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

LRESULT CChildView::OnMsgVideoSignalChanged(WPARAM w, LPARAM l)
{
	if (m_bRecord == TRUE)
		OnFileStoprecord();

	ClosePreview();

	OpenPreview(m_nChannel);

	return 0;
}

LRESULT CChildView::OnMsgVideoRenderReset(WPARAM w, LPARAM l)
{
	//reset render
	if (m_bRecord == TRUE)
		OnFileStoprecord();

	ClosePreview();

	OpenPreview(m_nChannel);

	return 0;
}

BOOL CChildView::OpenPreview(int nIndex)
{
	BOOL bRet = FALSE;
	if (!m_previewThread.Create(MWCAP_VIDEO_COLOR_FORMAT_YUV709,
				    MWCAP_VIDEO_QUANTIZATION_LIMITED, nIndex,
				    CAPTURE_WIDTH, CAPTURE_HEIGHT,
				    PREVIEW_FOURCC, PREVIEW_DURATION, this,
				    TRUE, PREVIEW_THREAD, m_nClip)) {
		AfxMessageBox(_T("Open device error!"));
		return FALSE;
	} else {
		int nWidth, nHeight;
		MWCAP_VIDEO_COLOR_FORMAT nColorFormat;
		MWCAP_VIDEO_QUANTIZATION_RANGE nQuantRange;
		BOOL bVideoValid = m_previewThread.GetCaptureVideoFormat(
			nWidth, nHeight, nColorFormat, nQuantRange);
		if (bVideoValid) {
			CRect rcWnd;
			rcWnd.SetRect(0, 0, nWidth, nHeight);
			AdjustWindowRectEx(&rcWnd, WS_OVERLAPPEDWINDOW, TRUE,
					   WS_EX_CLIENTEDGE);
			AfxGetMainWnd()->SetWindowPos(
				NULL, 0, 0, rcWnd.Width(), rcWnd.Height(),
				SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
		}

		CMWAutoLock lock(m_lockVideo);
		mw_render_init_t t_init = {0};
		t_init.m_u32_width = nWidth;
		t_init.m_u32_height = nHeight;
		t_init.m_mode = PREVIEW_FOURCC;
		m_hwnd = GetSafeHwnd();
		if (MWCAP_VIDEO_COLOR_FORMAT_YUV601 == nColorFormat)
			t_init.m_csp_in = MW_CSP_BT_601;
		else if (MWCAP_VIDEO_COLOR_FORMAT_YUV709 == nColorFormat)
			t_init.m_csp_in = MW_CSP_BT_709;
		else if (MWCAP_VIDEO_COLOR_FORMAT_YUV2020 == nColorFormat)
			t_init.m_csp_in = MW_CSP_BT_2020;
		else
			t_init.m_csp_in = MW_CSP_BT_709;

		if (MWCAP_VIDEO_QUANTIZATION_FULL == nQuantRange)
			t_init.m_clr_rng_in = MW_CSP_LEVELS_PC;
		else if (MWCAP_VIDEO_QUANTIZATION_LIMITED == nQuantRange)
			t_init.m_clr_rng_in = MW_CSP_LEVELS_TV;
		else
			t_init.m_clr_rng_in = MW_CSP_LEVELS_TV;

		t_init.m_clr_rng_out = MW_CSP_LEVELS_PC;

		if (!m_p_VideoRenderThread->open_render(&t_init, m_hwnd)) {
			m_p_VideoRenderThread->close();
			return FALSE;
		}

		BOOL bLPCM = FALSE;
		BYTE cBitsPerSample = 0;
		DWORD dwSampleRate = 0;
		if (!m_previewThread.GetInputAudioFormat(bLPCM, cBitsPerSample,
							 dwSampleRate)) {
			bLPCM = FALSE;
		}

		if (bLPCM) {
			CMWAutoLock lock(m_lockAudio);
			BOOL bRet = FALSE;
			do {
				m_pAudioRenderer = new CDSoundRenderer();
				if (!m_pAudioRenderer)
					break;

				GUID guid = GUID_NULL;
				if (!m_pAudioRenderer->Initialize(&guid))
					break;

				if (!m_pAudioRenderer->Create(
					    dwSampleRate, 2, 192,
					    DEF_AUDIO_JITTER_FRAMES))
					break;

				if (!m_pAudioRenderer->Run())
					break;

				bRet = TRUE;
			} while (FALSE);

			if (!bRet) {
				m_pAudioRenderer->Deinitialize();
				delete m_pAudioRenderer;
				m_pAudioRenderer = NULL;
			}
		}
	}
	m_nChannel = nIndex;

	return TRUE;
}

void CChildView::ClosePreview()
{
	m_previewThread.Destroy();

	if (m_p_VideoRenderThread != NULL) {
		m_p_VideoRenderThread->close();
	}

	if (m_pAudioRenderer) {
		m_pAudioRenderer->Deinitialize();
		delete m_pAudioRenderer;
		m_pAudioRenderer = NULL;
	}
}

UINT CChildView::OnPowerBroadcast(UINT nPowerEvent, UINT nEventData)
{
	if (nPowerEvent == PBT_POWERSETTINGCHANGE) {
		PPOWERBROADCAST_SETTING pSetting =
			(PPOWERBROADCAST_SETTING)nEventData;
		DWORD *pdwData = (DWORD *)pSetting->Data;
		if (*pdwData != 0x00) {
			OpenPreview(m_nChannel);
		} else {
			ClosePreview();
		}
	}

	return __super::OnPowerBroadcast(nPowerEvent, nEventData);
}

LRESULT CChildView::OnMsgDiskSizeOver(WPARAM w, LPARAM l)
{
	mw_disk_size_t t_disk_size;
	BOOL t_ret = get_disk_size(t_disk_size);
	if (t_ret) {
		if (t_disk_size.m_u32_gb < 1 && t_disk_size.m_u32_mb < 200) {
			OnFileStoprecord();
			TCHAR szLog[512] = {0};
			swprintf_s(
				szLog,
				_T("'%s' current free disk size is not enough."),
				m_wcs_dir);
			AfxMessageBox(szLog);
			return 0;
		}
	}

	return 0;
}

void CChildView::OnClose()
{
	__super::OnClose();
}

void CChildView::OnFileNoclip()
{
	m_nClip = CLIP_NONE;
	ClosePreview();
	OpenPreview(m_nChannel);
}

void CChildView::OnClipTopleft()
{
	m_nClip = CLIP_TOPLEFT;
	ClosePreview();
	OpenPreview(m_nChannel);
}

void CChildView::OnClipTopright()
{
	m_nClip = CLIP_TOPRIGHT;
	ClosePreview();
	OpenPreview(m_nChannel);
}

void CChildView::OnClipBottomleft()
{
	m_nClip = CLIP_BOTTOMLEFT;
	ClosePreview();
	OpenPreview(m_nChannel);
}

void CChildView::OnClipBottomright()
{
	m_nClip = CLIP_BOTTOMRIGHT;
	ClosePreview();
	OpenPreview(m_nChannel);
}

void CChildView::OnUpdateFileNoclip(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nClip == CLIP_NONE);
}

void CChildView::OnUpdateClipTopleft(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nClip == CLIP_TOPLEFT);
}

void CChildView::OnUpdateClipTopright(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nClip == CLIP_TOPRIGHT);
}

void CChildView::OnUpdateClipBottomleft(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nClip == CLIP_BOTTOMLEFT);
}

void CChildView::OnUpdateClipBottomright(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nClip == CLIP_BOTTOMRIGHT);
}

BOOL CChildView::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN) &&
	    pMsg->wParam)

		return TRUE;

	return __super::PreTranslateMessage(pMsg);
}

void CChildView::OnFileStoprecord()
{
	// TODO: Add your command handler code here
	m_bRecord = FALSE;

	if (m_hEncodeThread != NULL && m_hExitEncodeThread != NULL) {
		SetEvent(m_hExitEncodeThread);
		WaitForSingleObject(m_hEncodeThread, INFINITE);

		if (m_hExitEncodeThread != NULL) {
			CloseHandle(m_hExitEncodeThread);
			m_hExitEncodeThread = NULL;
		}

		if (m_hEncodeThread != NULL) {
			CloseHandle(m_hEncodeThread);
			m_hEncodeThread = NULL;
		}

		if (m_hEncodeFrame != NULL) {
			CloseHandle(m_hEncodeFrame);
			m_hEncodeFrame = NULL;
		}
	}

	m_FileRecord.Destroy();
}

void CChildView::OnUpdateFileStoprecord(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bRecord);
}