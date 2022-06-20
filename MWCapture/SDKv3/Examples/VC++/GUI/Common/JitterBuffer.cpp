#include "StdAfx.h"
#include "JitterBuffer.h"

#include <stdlib.h>

CJitterBuffer::CJitterBuffer(void)
	: m_cbMaxFrame(0),
	  m_cFrameCount(0),
	  m_nBufferCount(0),
	  m_cBufferingDuration(0),
	  m_cDefBufferingDuration(0),
	  m_cMinBufferingDuration(0),
	  m_cMaxBufferingDuration(0),
	  m_nBufferingLevel(0),
	  m_nLastBufferingLevel(0),
	  m_nLastJitter(0),
	  m_nMaxHistJitter(0),
	  m_nStableHist(0),
	  m_nLastOperation(OP_INIT),
	  m_pbFrameBuffer(NULL),
	  m_pFrameRingBuffer(NULL)
{
}

CJitterBuffer::~CJitterBuffer(void)
{
	Destroy();
}

BOOL CJitterBuffer::Create(int cbMaxFrame, int cFrameCount)
{
	m_cbMaxFrame = cbMaxFrame;
	m_cFrameCount = cFrameCount;
	m_cMinBufferingDuration = 1;
	m_cBufferingDuration = m_cDefBufferingDuration =
		max(m_cMinBufferingDuration, cFrameCount / 2);
	m_cMaxBufferingDuration =
		max(m_cMinBufferingDuration, cFrameCount * 4 / 5);

	m_pbFrameBuffer =
		(LPBYTE)_aligned_malloc((cFrameCount + 1) * cbMaxFrame, 16);
	m_pFrameRingBuffer = new FRAME[cFrameCount];
	if (NULL == m_pbFrameBuffer || NULL == m_pFrameRingBuffer) {
		Destroy();
		return FALSE;
	}

	Reset();
	return TRUE;
}

void CJitterBuffer::Reset()
{
	m_bEmpty = TRUE;
	m_nRingHead = m_nRingTail = 0;
	m_nLastSeqNo = m_nOrigin = -1;
	m_nBufferCount = 0;

	m_nBufferingLevel = m_nLastBufferingLevel = 0;
	m_nLastJitter = m_nMaxHistJitter = 0;
	m_nStableHist = 0;
	m_nLastOperation = OP_INIT;
	m_cBufferingDuration = m_cDefBufferingDuration;

	for (int i = 0; i < m_cFrameCount; i++) {
		LPBYTE pbFrame = m_pbFrameBuffer + i * m_cbMaxFrame;
		m_pFrameRingBuffer[i].m_pbFrame = pbFrame;
		m_pFrameRingBuffer[i].m_nType = JB_MISSING_FRAME;
		m_pFrameRingBuffer[i].m_cbFrame = 0;
	}

	m_frameLastOutput.m_pbFrame =
		m_pbFrameBuffer + m_cFrameCount * m_cbMaxFrame;
	m_frameLastOutput.m_nType = JB_MISSING_FRAME;
	m_frameLastOutput.m_cbFrame = 0;
}

void CJitterBuffer::Destroy()
{
	if (m_pFrameRingBuffer) {
		delete[] m_pFrameRingBuffer;
		m_pFrameRingBuffer = NULL;
	}

	if (m_pbFrameBuffer) {
		_aligned_free(m_pbFrameBuffer);
		m_pbFrameBuffer = NULL;
	}
}

BOOL CJitterBuffer::SetDuration(int cFrames)
{
	int cBufferingDuration = max(0, min(cFrames, m_cFrameCount));
	SetDuration(cBufferingDuration, cBufferingDuration, cBufferingDuration);
	return TRUE;
}

BOOL CJitterBuffer::SetDuration(int cFrames, int cMinFrames, int cMaxFrames)
{
	m_cMinBufferingDuration = max(1, cMinFrames);
	m_cMaxBufferingDuration =
		max(m_cMinBufferingDuration, min(cMaxFrames, m_cFrameCount));
	m_cDefBufferingDuration = m_cBufferingDuration = min(
		max(cFrames, m_cMinBufferingDuration), m_cMaxBufferingDuration);
	return TRUE;
}

BOOL CJitterBuffer::PutFrame(const BYTE *pbFrame, int cbFrame, int nFrameSeq)
{
	if (cbFrame > m_cbMaxFrame)
		return FALSE;

	if (m_nLastSeqNo == -1) {
		m_nLastSeqNo = nFrameSeq - 1;
	}

	int nSeqDiff = nFrameSeq - m_nLastSeqNo;
	m_nLastSeqNo = max(m_nLastSeqNo, nFrameSeq);

	if (nSeqDiff > 0) {
		m_nBufferingLevel += nSeqDiff;
		UpdateStatistics(OP_PUT);

		while (!PutFrameImpl(pbFrame, cbFrame, nFrameSeq)) {
			RemoveHead();
		}

		return TRUE;
	} else {
		return PutFrameImpl(pbFrame, cbFrame, nFrameSeq);
	}
}

int CJitterBuffer::GetFrame(LPBYTE pbFrame, int &cbFrame)
{
	m_nBufferingLevel--;
	UpdateStatistics(OP_GET);

	int cBuffering = GetBufferingCount();
	if (cBuffering <= 0) {
		m_nBufferCount = 0;
	}

	if (m_nBufferCount < m_cBufferingDuration) {
		return (m_nBufferCount == 0) ? JB_EMPTY_FRAME
					     : JB_BUFFERING_FRAME;
	}

	int nFrameType = m_pFrameRingBuffer[m_nRingHead].m_nType;
	if (nFrameType == JB_NORMAL_FRAME) {
		cbFrame = m_pFrameRingBuffer[m_nRingHead].m_cbFrame;
		memcpy(pbFrame, m_pFrameRingBuffer[m_nRingHead].m_pbFrame,
		       cbFrame);

		SwapFrameBuffer(m_frameLastOutput,
				m_pFrameRingBuffer[m_nRingHead]);
	}

	RemoveHead();
	return nFrameType;
}

BYTE *CJitterBuffer::BeginPutFrame(int cbFrame, int nFrameSeq)
{
	if (cbFrame > m_cbMaxFrame)
		return FALSE;

	if (m_nLastSeqNo == -1) {
		m_nLastSeqNo = nFrameSeq - 1;
	}

	int nSeqDiff = nFrameSeq - m_nLastSeqNo;
	m_nLastSeqNo = max(m_nLastSeqNo, nFrameSeq);

	if (nSeqDiff > 0) {
		m_nBufferingLevel += nSeqDiff;
		UpdateStatistics(OP_PUT);

		BYTE *pbFrame = NULL;
		while ((pbFrame = PutFrameImpl(cbFrame, nFrameSeq)) == NULL) {
			RemoveHead();
		}

		return pbFrame;
	} else {
		return PutFrameImpl(cbFrame, nFrameSeq);
	}
}

void CJitterBuffer::EndPutFrame() {}

int CJitterBuffer::BeginGetFrame(const BYTE *&pbFrame, int &cbFrame)
{
	m_nBufferingLevel--;
	UpdateStatistics(OP_GET);

	int cBuffering = GetBufferingCount();
	if (cBuffering <= 0) {
		m_nBufferCount = 0;
	}

	if (m_nBufferCount < m_cBufferingDuration) {
		return (m_nBufferCount == 0) ? JB_EMPTY_FRAME
					     : JB_BUFFERING_FRAME;
	}

	int nFrameType = m_pFrameRingBuffer[m_nRingHead].m_nType;
	if (nFrameType == JB_NORMAL_FRAME) {
		cbFrame = m_pFrameRingBuffer[m_nRingHead].m_cbFrame;
		pbFrame = m_pFrameRingBuffer[m_nRingHead].m_pbFrame;
		SwapFrameBuffer(m_frameLastOutput,
				m_pFrameRingBuffer[m_nRingHead]);
	}

	RemoveHead();
	return nFrameType;
}

void CJitterBuffer::EndGetFrame() {}

BOOL CJitterBuffer::GetLastOutputFrame(LPBYTE pbFrame, int &cbFrame)
{
	if (m_frameLastOutput.m_nType != JB_NORMAL_FRAME)
		return FALSE;

	cbFrame = m_frameLastOutput.m_cbFrame;
	memcpy(pbFrame, m_frameLastOutput.m_pbFrame, cbFrame);
	return TRUE;
}

LPBYTE CJitterBuffer::GetLastOutputFrame(int &cbFrame)
{
	if (m_frameLastOutput.m_nType != JB_NORMAL_FRAME)
		return NULL;

	cbFrame = m_frameLastOutput.m_cbFrame;
	return m_frameLastOutput.m_pbFrame;
}

int CJitterBuffer::GetBufferingCount()
{
	if (m_nRingTail == m_nRingHead) {
		return m_bEmpty ? 0 : m_cFrameCount;
	} else {
		return (m_nRingTail - m_nRingHead + m_cFrameCount) %
		       m_cFrameCount;
	}
}

void CJitterBuffer::RemoveHead()
{
	m_pFrameRingBuffer[m_nRingHead].m_nType = JB_MISSING_FRAME;
	m_pFrameRingBuffer[m_nRingHead].m_cbFrame = 0;

	m_nOrigin++;
	m_nRingHead = (m_nRingHead + 1) % m_cFrameCount;
	if (m_nRingHead == m_nRingTail)
		m_bEmpty = TRUE;
}

BOOL CJitterBuffer::PutFrameImpl(const BYTE *pbFrame, int cbFrame,
				 int nFrameSeq)
{
	int nPutPos = 0;

	if (m_bEmpty) {
		nPutPos = m_nRingTail;
		m_nOrigin = nFrameSeq;
		m_nRingTail = (m_nRingTail + 1) % m_cFrameCount;
		m_nBufferCount++;
		m_bEmpty = FALSE;
	} else {
		if (nFrameSeq < m_nOrigin) {
			return FALSE;
		}

		int nMaxSeq = m_nOrigin + m_cFrameCount - 1;
		if (nFrameSeq > nMaxSeq) {
			return FALSE;
		}

		nPutPos = (nFrameSeq - m_nOrigin + m_nRingHead) % m_cFrameCount;

		int nCurSize = GetBufferingCount();
		if (nFrameSeq >= m_nOrigin + nCurSize) {
			int nDiff = nFrameSeq - (m_nOrigin + nCurSize) + 1;
			m_nRingTail = (m_nRingTail + nDiff) % m_cFrameCount;
			m_nBufferCount += nDiff;
		}
	}

	memcpy(m_pFrameRingBuffer[nPutPos].m_pbFrame, pbFrame, cbFrame);
	m_pFrameRingBuffer[nPutPos].m_cbFrame = cbFrame;
	m_pFrameRingBuffer[nPutPos].m_nType = JB_NORMAL_FRAME;
	return TRUE;
}

BYTE *CJitterBuffer::PutFrameImpl(int cbFrame, int nFrameSeq)
{
	int nPutPos = 0;

	if (m_bEmpty) {
		nPutPos = m_nRingTail;
		m_nOrigin = nFrameSeq;
		m_nRingTail = (m_nRingTail + 1) % m_cFrameCount;
		m_nBufferCount++;
		m_bEmpty = FALSE;
	} else {
		if (nFrameSeq < m_nOrigin) {
			return NULL;
		}

		int nMaxSeq = m_nOrigin + m_cFrameCount - 1;
		if (nFrameSeq > nMaxSeq) {
			return NULL;
		}

		nPutPos = (nFrameSeq - m_nOrigin + m_nRingHead) % m_cFrameCount;

		int nCurSize = GetBufferingCount();
		if (nFrameSeq >= m_nOrigin + nCurSize) {
			int nDiff = nFrameSeq - (m_nOrigin + nCurSize) + 1;
			m_nRingTail = (m_nRingTail + nDiff) % m_cFrameCount;
			m_nBufferCount += nDiff;
		}
	}

	m_pFrameRingBuffer[nPutPos].m_cbFrame = cbFrame;
	m_pFrameRingBuffer[nPutPos].m_nType = JB_NORMAL_FRAME;
	return m_pFrameRingBuffer[nPutPos].m_pbFrame;
}

void CJitterBuffer::UpdateStatistics(int nOperation)
{
	if (m_nLastOperation != nOperation) {
		CalculateJitter();
		m_nLastOperation = nOperation;
	}
}

void CJitterBuffer::CalculateJitter()
{
	m_nLastJitter = abs(m_nBufferingLevel - m_nLastBufferingLevel);
	m_nLastBufferingLevel = m_nBufferingLevel;
	m_nMaxHistJitter = max(m_nMaxHistJitter, m_nLastJitter);

	if (m_nLastJitter < m_cBufferingDuration) {
		m_nStableHist++;
		if (m_nStableHist > 50) {
			int nDiff = max(
				1,
				(m_cBufferingDuration - m_nMaxHistJitter) / 3);
			int cBufferingDuration =
				max(m_cBufferingDuration - nDiff,
				    m_cMinBufferingDuration);

			if (cBufferingDuration != m_cBufferingDuration) {
				//TRACE(_T("New Buffer Druation: %d, Jitter: %d\n"), m_cBufferingDuration, m_nLastJitter);
				m_cBufferingDuration = cBufferingDuration;
			}

			m_nStableHist = 0;
			m_nMaxHistJitter = 0;
		}
	} else {
		int cBufferingDuration =
			min(m_nLastJitter, m_cMaxBufferingDuration);

		if (cBufferingDuration != m_cBufferingDuration) {
			//TRACE(_T("New Buffer Druation: %d, Jitter: %d\n"), m_cBufferingDuration, m_nLastJitter);
			m_cBufferingDuration = cBufferingDuration;
		}

		m_nStableHist = 0;
		m_nMaxHistJitter = 0;
	}
}

void CJitterBuffer::SwapFrameBuffer(CJitterBuffer::FRAME &frame1,
				    CJitterBuffer::FRAME &frame2)
{
	FRAME frameTemp;
	frameTemp = frame1;
	frame1 = frame2;
	frame2 = frameTemp;
}
