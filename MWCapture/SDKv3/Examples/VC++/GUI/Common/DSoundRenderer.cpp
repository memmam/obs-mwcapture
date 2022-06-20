#include "StdAfx.h"
#include "DSoundRenderer.h"

#define HARDWARE_PLAYBACK_BUFFER_SIZE 0

CDSoundRenderer::CDSoundRenderer(void)
	: m_bRunning(FALSE),
	  m_hThread(NULL),
	  m_lpDS(NULL),
	  m_lpDSBuf(NULL),
	  m_hEvNotify(NULL),
	  m_lpDSNotify(NULL),
	  m_cbFrameSize(0),
	  m_cSamplesPerFrame(0),
	  m_cbBufferSize(0),
	  m_dwWritePos(0),
	  m_nFrameSeq(0),
	  m_cEmptyCount(0)
{
	memset(&m_wfx, 0, sizeof(m_wfx));
}

CDSoundRenderer::~CDSoundRenderer(void)
{
	Deinitialize();
}

BOOL CDSoundRenderer::Initialize(LPCGUID lpGUID)
{
	return SUCCEEDED(DirectSoundCreate(lpGUID, &m_lpDS, NULL));
}

void CDSoundRenderer::Deinitialize()
{
	Destroy();

	if (NULL != m_lpDS) {
		m_lpDS->Release();
		m_lpDS = NULL;
	}
}

BOOL CDSoundRenderer::Create(int nSamplesPerSec, int nChannels,
			     int nSamplesPerFrame, int nFrameCount)
{
	if (NULL == m_lpDS)
		return FALSE;

	nFrameCount = min(nFrameCount, MAX_FRAME_BUFFER_COUNT);
	nFrameCount = max(nFrameCount, MIN_FRAME_BUFFER_COUNT);

	memset(&m_wfx, 0, sizeof(m_wfx));
	m_wfx.wFormatTag = WAVE_FORMAT_PCM;
	m_wfx.nChannels = nChannels;
	m_wfx.wBitsPerSample = 16;
	m_wfx.nSamplesPerSec = nSamplesPerSec;
	m_wfx.nAvgBytesPerSec =
		nSamplesPerSec * nChannels * m_wfx.wBitsPerSample / 8;
	m_wfx.nBlockAlign = nChannels * m_wfx.wBitsPerSample / 8;
	m_wfx.cbSize = 0;

	m_cSamplesPerFrame = nSamplesPerFrame;
	m_cbFrameSize = nSamplesPerFrame * nChannels * m_wfx.wBitsPerSample / 8;
	nFrameCount = (nFrameCount * m_cbFrameSize +
		       HARDWARE_PLAYBACK_BUFFER_SIZE + m_cbFrameSize - 1) /
		      m_cbFrameSize;
	m_cbBufferSize = nFrameCount * m_cbFrameSize;

	DSBUFFERDESC dsbd;
	ZeroMemory(&dsbd, sizeof(dsbd));
	dsbd.dwSize = sizeof(dsbd);
	dsbd.dwBufferBytes = m_cbBufferSize;
	dsbd.lpwfxFormat = &m_wfx;
	dsbd.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPOSITIONNOTIFY |
		       DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;

	HWND hwnd = GetForegroundWindow();
	if (NULL == hwnd) {
		hwnd = GetDesktopWindow();
	}

	m_hEvNotify = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == m_hEvNotify) {
		Destroy();
		return FALSE;
	}

	DSBPOSITIONNOTIFY dsPosNotify[MAX_FRAME_BUFFER_COUNT];
	for (int i = 0; i < nFrameCount; i++) {
		dsPosNotify[i].dwOffset =
			i * m_cbFrameSize + (m_cbFrameSize - 1);
		dsPosNotify[i].hEventNotify = m_hEvNotify;
	}

	if (FAILED(m_lpDS->SetCooperativeLevel(hwnd, DSSCL_PRIORITY)) ||
	    FAILED(m_lpDS->CreateSoundBuffer(&dsbd, &m_lpDSBuf, NULL)) ||
	    FAILED(m_lpDSBuf->QueryInterface(IID_IDirectSoundNotify,
					     (LPVOID *)&m_lpDSNotify)) ||
	    FAILED(m_lpDSNotify->SetNotificationPositions(nFrameCount,
							  dsPosNotify)) ||
	    FAILED(m_lpDSBuf->SetCurrentPosition(0))) {
		Destroy();
		return FALSE;
	}

	SetVolume(DSBVOLUME_MAX);

	m_dwWritePos = 0;

	// Jitter
	if (!m_buffer.Create(m_cbFrameSize, MAX_AUDIO_JITTER_FRAMES))
		return FALSE;

	if (!m_buffer.SetDuration(DEF_AUDIO_JITTER_FRAMES))
		return FALSE;

	return TRUE;
}

void CDSoundRenderer::Destroy()
{
	Stop();

	if (NULL != m_lpDSBuf) {
		m_lpDSBuf->Release();
		m_lpDSBuf = NULL;
	}

	if (NULL != m_lpDSNotify) {
		m_lpDSNotify->Release();
		m_lpDSNotify = NULL;
	}

	if (m_hEvNotify) {
		CloseHandle(m_hEvNotify);
		m_hEvNotify = NULL;
	}
}

BOOL CDSoundRenderer::Run()
{
	if (m_bRunning)
		return FALSE;

	m_lpDSBuf->SetCurrentPosition(0);
	m_dwWritePos = 0;
	WriteWaveform(TRUE);

	DWORD dwThreadId = 0;
	m_bRunning = TRUE;
	m_hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &dwThreadId);
	if (NULL == m_hThread ||
	    FAILED(m_lpDSBuf->Play(0, 0, DSBPLAY_LOOPING))) {
		Stop();
		return FALSE;
	}

	return TRUE;
}

void CDSoundRenderer::Stop()
{
	if (!m_bRunning || m_lpDSBuf == NULL)
		return;

	m_lpDSBuf->Stop();

	m_bRunning = FALSE;
	if (m_hThread) {
		SetEvent(m_hEvNotify);
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);
		m_hThread = NULL;
	}
}

BOOL CDSoundRenderer::SetPan(LONG lPan)
{
	return (SUCCEEDED(m_lpDSBuf->SetPan(lPan)));
}

BOOL CDSoundRenderer::SetVolume(LONG lVolume)
{
	return (SUCCEEDED(m_lpDSBuf->SetVolume(lVolume)));
}

DWORD WINAPI CDSoundRenderer::ThreadProc(LPVOID lpvParam)
{
	CDSoundRenderer *pThis = (CDSoundRenderer *)lpvParam;
	return pThis->ThreadProc();
}

DWORD CDSoundRenderer::ThreadProc()
{
	SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);

	while (m_bRunning) {
		DWORD dwRet = WaitForSingleObject(m_hEvNotify, INFINITE);
		if (dwRet != WAIT_OBJECT_0 || !WriteWaveform())
			break;
	}

	return 0;
}

BOOL CDSoundRenderer::WriteWaveform(BOOL bSilence)
{
	DWORD dwReadPos, dwWritePos;
	HRESULT hr = m_lpDSBuf->GetCurrentPosition(&dwReadPos, &dwWritePos);
	if (FAILED(hr)) {
		return FALSE;
	}

	DWORD cbAvail;
	if (m_dwWritePos <= dwReadPos)
		cbAvail = dwReadPos - m_dwWritePos;
	else
		cbAvail = m_cbBufferSize - m_dwWritePos + dwReadPos;

	cbAvail = (cbAvail / m_cbFrameSize) * m_cbFrameSize;
	if (cbAvail <= 0)
		return TRUE;

	LPVOID pbData1, pbData2;
	DWORD cbData1, cbData2;
	hr = m_lpDSBuf->Lock(m_dwWritePos, cbAvail, &pbData1, &cbData1,
			     &pbData2, &cbData2, 0);
	if (DSERR_BUFFERLOST == hr) {
		m_lpDSBuf->Restore();
		hr = m_lpDSBuf->Lock(m_dwWritePos, cbAvail, &pbData1, &cbData1,
				     &pbData2, &cbData2, 0);
	}

	if (FAILED(hr)) {
		//_TRACE(_T("Lock Failed: %08X, POS: %d, Len: %d\n"), hr, m_dwWritePos, cbAvail);
		return FALSE;
	}

	if (bSilence) {
		memset(pbData1, 0, cbData1);
		if (cbData2)
			memset(pbData2, 0, cbData2);
		m_dwWritePos = m_cbBufferSize - m_cbFrameSize;
	} else {
		LPBYTE pbData;
		DWORD cbData;
		for (pbData = (LPBYTE)pbData1, cbData = cbData1;
		     m_cbFrameSize <= cbData && m_bRunning;
		     cbData -= m_cbFrameSize) {
			GetFrameSamples((short *)pbData);
			pbData += m_cbFrameSize;
		}

		for (pbData = (LPBYTE)pbData2, cbData = cbData2;
		     m_cbFrameSize <= cbData && m_bRunning;
		     cbData -= m_cbFrameSize) {
			GetFrameSamples((short *)pbData);
			pbData += m_cbFrameSize;
		}

		m_dwWritePos = (m_dwWritePos + cbAvail) % m_cbBufferSize;
	}

	m_lpDSBuf->Unlock(pbData1, cbData1, pbData2, cbData2);
	return TRUE;
}