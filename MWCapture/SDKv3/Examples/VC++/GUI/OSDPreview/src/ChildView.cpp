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

#include "OSDPreview.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CAPTURE_WIDTH 1920
#define CAPTURE_HEIGHT 1080

#define CAPTURE_DURATION 400000

#define CAPTURE_COLOR MWFOURCC_YUY2

// CChildView
CChildView::CChildView()
{
	m_hChannel = NULL;

	m_hThread = NULL;
	m_hExitThread = NULL;

	m_pRenderer = NULL;

	m_bBorder = FALSE;
	m_bText = FALSE;
	m_bIcon = TRUE;

	m_nIconIndex = 0;
	m_colorFormat = MWCAP_VIDEO_COLOR_FORMAT_YUV709;
	m_quantRange = MWCAP_VIDEO_QUANTIZATION_LIMITED;
}

CChildView::~CChildView() {}

BEGIN_MESSAGE_MAP(CChildView, CWnd)
ON_WM_PAINT()
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()
ON_WM_ERASEBKGND()
ON_COMMAND(ID_FILE_TEXT, &CChildView::OnFileText)
ON_UPDATE_COMMAND_UI(ID_FILE_TEXT, &CChildView::OnUpdateFileText)
ON_COMMAND(ID_FILE_BORDER, &CChildView::OnFileBorder)
ON_UPDATE_COMMAND_UI(ID_FILE_BORDER, &CChildView::OnUpdateFileBorder)
ON_COMMAND(ID_FILE_ICON, &CChildView::OnFileIcon)
ON_UPDATE_COMMAND_UI(ID_FILE_ICON, &CChildView::OnUpdateFileIcon)
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

BOOL CChildView::LoadImageFromResource(Bitmap **ppImage, UINT nResID,
				       LPCTSTR lpTyp)
{
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(),
				     MAKEINTRESOURCE(nResID), lpTyp);
	if (hRsrc == NULL)
		return FALSE;

	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
	if (hImgData == NULL)
		return FALSE;

	LPVOID lpVoid = ::LockResource(hImgData);

	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	::GlobalUnlock(hNew);

	IStream *pstm;
	CreateStreamOnHGlobal(hNew, FALSE, &pstm);
	*ppImage = Gdiplus::Bitmap::FromStream(pstm);
	pstm->Release();

	GlobalFree(hNew);

	::FreeResource(hImgData);

	return TRUE;
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	for (int i = 0; i < 5; i++) {
		int nResID = IDB_PNG1 + i;
		if (!LoadImageFromResource(&m_pIcon[i], IDB_PNG1 + i,
					   _T("PNG"))) {
			OutputDebugString(_T("LoadImageFromResource fail\n"));
			break;
		}
	}

	mw_color_space_e color_format;
	mw_color_space_level_e input_range;
	mw_color_space_level_e output_range = MW_CSP_LEVELS_PC;
	if (MWCAP_VIDEO_COLOR_FORMAT_YUV601 == m_colorFormat)
		color_format = MW_CSP_BT_601;
	else if (MWCAP_VIDEO_COLOR_FORMAT_YUV709 == m_colorFormat)
		color_format = MW_CSP_BT_709;
	else if (MWCAP_VIDEO_COLOR_FORMAT_YUV2020 == m_colorFormat)
		color_format = MW_CSP_BT_2020;
	else
		color_format = MW_CSP_BT_709;

	if (MWCAP_VIDEO_QUANTIZATION_FULL == m_quantRange)
		input_range = MW_CSP_LEVELS_PC;
	else if (MWCAP_VIDEO_QUANTIZATION_LIMITED == m_quantRange)
		input_range = MW_CSP_LEVELS_TV;
	else
		input_range = MW_CSP_LEVELS_TV;

	m_pRenderer = new MWDXRender();
	bool t_b_reverse = false;
	m_pRenderer->initialize(CAPTURE_WIDTH, CAPTURE_HEIGHT, CAPTURE_COLOR,
				t_b_reverse, GetSafeHwnd(), color_format,
				input_range, output_range);
	if (m_pRenderer == NULL)
		return -1;

	return 0;
}

void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// Do not call CWnd::OnPaint() for painting messages
}

void CChildView::OnDestroy()
{
	if (m_hExitThread != NULL && m_hThread != NULL) {
		SetEvent(m_hExitThread);
		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hExitThread);
		m_hExitThread = NULL;

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	if (m_pRenderer) {
		m_pRenderer->cleanup_device();
		delete m_pRenderer;
		m_pRenderer = NULL;
	}

	CWnd::OnDestroy();
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
}

BOOL CChildView::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

BOOL CChildView::SelectChannel(int nIndex)
{
	if (m_hExitThread != NULL && m_hThread != NULL) {
		SetEvent(m_hExitThread);
		WaitForSingleObject(m_hThread, INFINITE);

		CloseHandle(m_hExitThread);
		m_hExitThread = NULL;

		CloseHandle(m_hThread);
		m_hThread = NULL;
	}

	if (m_hChannel != NULL) {
		MWCloseChannel(m_hChannel);
		m_hChannel = NULL;
	}

	do {
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
			break;
		}

		m_hChannel = MWOpenChannelByPath(szDevicePath);
		if (m_hChannel == NULL) {
			MessageBox(_T("Open video channel fail !\n"));
			break;
		}

		m_hExitThread = CreateEvent(NULL, FALSE, FALSE, NULL);

		m_hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)this, 0,
					 NULL);
		if (m_hThread == NULL) {
			break;
		}

	} while (FALSE);

	return TRUE;
}

DWORD CChildView::ThreadProc()
{
	SetPriorityClass(m_hThread, REALTIME_PRIORITY_CLASS);

	HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	DWORD cbStride = FOURCC_CalcMinStride(CAPTURE_COLOR, CAPTURE_WIDTH, 2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(CAPTURE_COLOR, CAPTURE_WIDTH,
						 CAPTURE_HEIGHT, cbStride);
	BYTE *byBuffer = new BYTE[dwFrameSize];

	m_dwFrameCount = 0;

	HOSD hOSD = MWCreateImage(m_hChannel, CAPTURE_WIDTH, CAPTURE_HEIGHT);

	MW_RESULT xr;
	do {
		xr = MWStartVideoCapture(m_hChannel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
		if (MW_SUCCEEDED !=
		    MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo))
			break;

		MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
		xr = MWGetVideoFrameInfo(
			m_hChannel, videoBufferInfo.iNewestBufferedFullFrame,
			&videoFrameInfo);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_SIGNAL_STATUS videoSignalStatus;
		xr = MWGetVideoSignalStatus(m_hChannel, &videoSignalStatus);
		if (xr != MW_SUCCEEDED)
			break;

		HTIMER hTimer = MWRegisterTimer(m_hChannel, hNotifyEvent);
		if (hTimer == NULL)
			break;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_hChannel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llExpireTime = llBegin;
		int i = 0;
		while (TRUE) {
			llExpireTime = llBegin + CAPTURE_DURATION;
			xr = MWScheduleTimer(m_hChannel, hTimer, llExpireTime);
			if (xr != MW_SUCCEEDED)
				continue;

			HANDLE aEvent[] = {m_hExitThread, hNotifyEvent};
			DWORD dwRet = WaitForMultipleObjects(2, aEvent, FALSE,
							     INFINITE);
			if (dwRet == WAIT_OBJECT_0)
				break;

			xr = MWGetVideoBufferInfo(m_hChannel, &videoBufferInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			xr = MWGetVideoFrameInfo(
				m_hChannel,
				videoBufferInfo.iNewestBufferedFullFrame,
				&videoFrameInfo);
			if (xr != MW_SUCCEEDED)
				continue;

			LoadOSD(hOSD);

			RECT aOSDRects[] = {
				{0, 0, CAPTURE_WIDTH, CAPTURE_HEIGHT}};
			xr = MWCaptureVideoFrameWithOSDToVirtualAddress(
				m_hChannel,
				MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED, byBuffer,
				dwFrameSize, cbStride, FALSE, NULL,
				CAPTURE_COLOR, CAPTURE_WIDTH, CAPTURE_HEIGHT,
				hOSD, aOSDRects, 1);
			// xr = MWCaptureVideoFrameToVirtualAddress(m_hChannel, videoBufferInfo.iNewestBufferedFullFrame, byBuffer, dwFrameSize, cbStride, FALSE, NULL, CAPTURE_COLOR, CAPTURE_WIDTH, CAPTURE_HEIGHT);

			WaitForSingleObject(hCaptureEvent, INFINITE);
			xr = MWGetVideoCaptureStatus(m_hChannel,
						     &captureStatus);

			if (m_pRenderer != NULL) {
				m_pRenderer->paint(byBuffer);
			}

			m_dwFrameCount++;
		}

		xr = MWUnregisterTimer(m_hChannel, hTimer);

		xr = MWStopVideoCapture(m_hChannel);

	} while (FALSE);

	if (hOSD != NULL) {
		LONG lRet = 0;
		MWCloseImage(m_hChannel, hOSD, &lRet);
	}

	delete[] byBuffer;

	CloseHandle(hNotifyEvent);
	CloseHandle(hCaptureEvent);

	return 0;
}

BOOL CChildView::LoadOSD(HOSD hOSD)
{
	int cx = CAPTURE_WIDTH;
	int cy = CAPTURE_HEIGHT;
	Gdiplus::Rect rect(0, 0, cx, cy);

	Gdiplus::Bitmap bitmapOSD(CAPTURE_WIDTH, CAPTURE_HEIGHT,
				  PixelFormat32bppARGB);

	Gdiplus::BitmapData bitmapDataOSD;

	if (m_bBorder) {
		Gdiplus::Pen penBlue(Color::White, 3);
		Gdiplus::Graphics graphicsOSD(&bitmapOSD);
		graphicsOSD.DrawRectangle(&penBlue, 10, 10, cx - 20, cy - 20);
	}

	if (m_bIcon) {
		int nWidth = m_pIcon[m_nIconIndex]->GetWidth();
		int nHeight = m_pIcon[m_nIconIndex]->GetHeight();

		Gdiplus::Graphics graphicsOSD(&bitmapOSD);
		graphicsOSD.DrawImage(m_pIcon[m_nIconIndex], cx - nWidth - 20,
				      20, nWidth, nHeight);

		if (m_dwFrameCount % 10 == 0)
			m_nIconIndex = (m_nIconIndex + 1) % 5;
	}

	Gdiplus::Status status = bitmapOSD.LockBits(&rect, ImageLockModeWrite,
						    PixelFormat32bppARGB,
						    &bitmapDataOSD);

	BOOL bOSDImageBottomUp;
	LPBYTE pbyOSDImage;
	int cbOSDImageStride;

	if (bitmapDataOSD.Stride < 0) {
		pbyOSDImage = ((LPBYTE)bitmapDataOSD.Scan0) +
			      bitmapDataOSD.Stride * (cy - 1);
		cbOSDImageStride = -bitmapDataOSD.Stride;
		bOSDImageBottomUp = TRUE;
	} else {
		pbyOSDImage = (LPBYTE)bitmapDataOSD.Scan0;
		cbOSDImageStride = bitmapDataOSD.Stride;
		bOSDImageBottomUp = FALSE;
	}

	if (hOSD != NULL) {
		MWUploadImageFromVirtualAddress(m_hChannel, hOSD, m_colorFormat,
						MWCAP_VIDEO_QUANTIZATION_FULL,
						MWCAP_VIDEO_SATURATION_FULL, 0,
						0, cx, cy,
						(MWCAP_PTR64)pbyOSDImage,
						cbOSDImageStride * cy,
						cbOSDImageStride, cx, cy,
						bOSDImageBottomUp, TRUE, TRUE);
	}

	bitmapOSD.UnlockBits(&bitmapDataOSD);

	if (m_bText) {
		LoadText(hOSD);
	}

	return (hOSD != NULL);
}

#define ALIGNED(x, y) ((x + y - 1) & ~(y - 1))

BOOL CChildView::LoadText(HOSD hOSD)
{
	Gdiplus::GraphicsPath path;
	FontFamily family(L"Times New Roman");

	INT style = FontStyleRegular;

	SYSTEMTIME st = {0};
	GetLocalTime(&st);

	TCHAR szTime[256];
	wsprintf(szTime, _T("%02d:%02d:%02d.%03d"), st.wHour, st.wMinute,
		 st.wSecond, st.wMilliseconds);

	path.AddString(szTime, -1, &family, style, (Gdiplus::REAL)32,
		       PointF(0, 0), StringFormat::GenericTypographic());

	Gdiplus::GraphicsPath *path2 = path.Clone();
	if (NULL == path2)
		return FALSE;

	Gdiplus::Pen pen(Color(0, 0, 0), (Gdiplus::REAL)2);
	path2->Widen(&pen);

	Gdiplus::Rect rcBound;
	path2->GetBounds(&rcBound);

	Gdiplus::Matrix mx;
	mx.Translate((Gdiplus::REAL)(-rcBound.X), (Gdiplus::REAL)(-rcBound.Y));
	path.Transform(&mx);
	path2->Transform(&mx);

	rcBound.Width = (rcBound.Width + 1) / 2 * 2;

	// for 4K card
	rcBound.Width = ALIGNED(rcBound.Width, 4);

	Gdiplus::Bitmap bitmapOSD(rcBound.Width, rcBound.Height,
				  PixelFormat32bppARGB);

	Gdiplus::Graphics graphics(&bitmapOSD);
	graphics.SetSmoothingMode(SmoothingModeHighQuality);

	Gdiplus::SolidBrush brush2(Color(255, 0, 0, 255));
	graphics.FillPath(&brush2, path2);
	delete path2;

	Gdiplus::SolidBrush brush(Color(255, 255, 0, 0));
	graphics.FillPath(&brush, &path);

	Gdiplus::Rect rect(0, 0, rcBound.Width, rcBound.Height);
	Gdiplus::BitmapData bitmapDataOSD;
	Gdiplus::Status status = bitmapOSD.LockBits(&rect, ImageLockModeWrite,
						    PixelFormat32bppARGB,
						    &bitmapDataOSD);

	BOOL bOSDImageBottomUp;
	LPBYTE pbyOSDImage;
	int cbOSDImageStride;

	int nImageStride = 0;
	BYTE *pbyImage = NULL;

	if (bitmapDataOSD.Stride < 0) {
		pbyOSDImage = ((LPBYTE)bitmapDataOSD.Scan0) +
			      bitmapDataOSD.Stride * (rcBound.Height - 1);
		cbOSDImageStride = -bitmapDataOSD.Stride;
		bOSDImageBottomUp = TRUE;
	} else {
		pbyOSDImage = (LPBYTE)bitmapDataOSD.Scan0;
		cbOSDImageStride = bitmapDataOSD.Stride;
		bOSDImageBottomUp = FALSE;

		// for 4K card
		pbyImage = (BYTE *)_aligned_malloc(
			cbOSDImageStride * rcBound.Height, 16);
		for (int i = 0; i < rcBound.Height; i++) {
			memcpy(pbyImage + i * cbOSDImageStride,
			       pbyOSDImage + cbOSDImageStride * i,
			       cbOSDImageStride);
		}
	}

	MW_RESULT xr;
	if (hOSD != NULL) {
		xr = MWUploadImageFromVirtualAddress(
			m_hChannel, hOSD, m_colorFormat,
			MWCAP_VIDEO_QUANTIZATION_FULL,
			MWCAP_VIDEO_SATURATION_FULL, 20, 20, rcBound.Width,
			rcBound.Height, (MWCAP_PTR64)pbyImage,
			cbOSDImageStride * rcBound.Height, cbOSDImageStride,
			rcBound.Width, rcBound.Height, bOSDImageBottomUp, TRUE,
			TRUE);
	}

	bitmapOSD.UnlockBits(&bitmapDataOSD);

	if (pbyImage != NULL) {
		_aligned_free(pbyImage);
		pbyImage = NULL;
	}

	return TRUE;
}

void CChildView::OnFileText()
{
	m_bText = !m_bText;
}

void CChildView::OnUpdateFileText(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bText);
}

void CChildView::OnFileBorder()
{
	m_bBorder = !m_bBorder;
}

void CChildView::OnUpdateFileBorder(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bBorder);
}

void CChildView::OnFileIcon()
{
	m_bIcon = !m_bIcon;
}

void CChildView::OnUpdateFileIcon(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_bIcon);
}

BOOL CChildView::PreTranslateMessage(MSG *pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam &&
	    (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE ||
	     pMsg->wParam == VK_TAB || pMsg->wParam == VK_LEFT ||
	     pMsg->wParam == VK_RIGHT || pMsg->wParam == VK_UP ||
	     pMsg->wParam == VK_DOWN))

		return TRUE;

	return CWnd::PreTranslateMessage(pMsg);
}
