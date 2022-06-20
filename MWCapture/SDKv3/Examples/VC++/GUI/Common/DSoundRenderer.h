#pragma once

#include <mmsystem.h>
#include <dsound.h>

#include <math.h>

#include "LockUtils.h"
#include "JitterBuffer.h"

#define MAX_FRAME_BUFFER_COUNT 128
#define MIN_FRAME_BUFFER_COUNT 3

#define MAX_AUDIO_JITTER_FRAMES 10
#define DEF_AUDIO_JITTER_FRAMES 10

class CDSoundRenderer {
public:
	CDSoundRenderer(void);
	virtual ~CDSoundRenderer(void);

public:
	BOOL Initialize(LPCGUID lpGUID);
	void Deinitialize();

public:
	BOOL Create(int nSamplesPerSec, int nChannels, int nSamplesPerFrame,
		    int nBufferFrameCount);
	void Destroy();

	BOOL Run();
	void Stop();

	BOOL SetPan(LONG lPan);
	BOOL SetVolume(LONG lVolume);

	BOOL PutFrame(const BYTE *pbFrame, int cbFrame)
	{
		if (cbFrame != m_cbFrameSize)
			return FALSE;

		CMWAutoLock lock(m_csBuffer);
		return m_buffer.PutFrame(pbFrame, cbFrame, m_nFrameSeq++);
	}

protected:
	static DWORD WINAPI ThreadProc(LPVOID lpvParam);
	virtual DWORD ThreadProc();
	BOOL WriteWaveform(BOOL bSilence = FALSE);

	void GetFrameSamples(short *psSamples)
	{
		CMWAutoLock lock(m_csBuffer);
		int cbFrame = 0;
		int nRet = m_buffer.GetFrame((LPBYTE)psSamples, cbFrame);

		switch (nRet) {
		case JB_NORMAL_FRAME:
			m_cEmptyCount = 0;
			break;

		case JB_MISSING_FRAME:
		case JB_EMPTY_FRAME:
		case JB_BUFFERING_FRAME:
			//_TRACE(_T("Audio Jitter Buffer: %d\n"), nRet);

			if (m_cEmptyCount < 5 &&
			    m_buffer.GetLastOutputFrame((LPBYTE)psSamples,
							cbFrame)) {
				double dfGainLinear =
					pow(10.0, -10.0 * m_cEmptyCount / 20.0);
				int cSamples = m_cbFrameSize / sizeof(short);
				for (int i = 0; i < cSamples; i++) {
					psSamples[i] = (short)(psSamples[i] *
							       dfGainLinear);
				}
				m_cEmptyCount++;
			} else {
				memset((LPBYTE)psSamples, 0, m_cbFrameSize);
			}
			cbFrame = m_cbFrameSize;
			break;
		}
	}

protected:
	BOOL m_bRunning;
	HANDLE m_hThread;

	WAVEFORMATEX m_wfx;
	LPDIRECTSOUND m_lpDS;
	LPDIRECTSOUNDBUFFER m_lpDSBuf;
	LPDIRECTSOUNDNOTIFY m_lpDSNotify;
	HANDLE m_hEvNotify;
	DWORD m_cbFrameSize;
	DWORD m_cSamplesPerFrame;
	DWORD m_cbBufferSize;
	DWORD m_dwWritePos;

	CJitterBuffer m_buffer;
	CMWLock m_csBuffer;
	int m_nFrameSeq;
	int m_cEmptyCount;
};
