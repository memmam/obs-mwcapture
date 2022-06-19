#pragma once
#include <Windows.h>

enum JB_FRAME_TYPE {
	JB_MISSING_FRAME,
	JB_NORMAL_FRAME,
	JB_EMPTY_FRAME,
	JB_BUFFERING_FRAME
};

class CJitterBuffer
{
public:
	CJitterBuffer(void);
	virtual ~CJitterBuffer(void);

public:
	BOOL Create(int cbFrame, int cFrameCount);
	void Reset();
	void Destroy();

public:
	BOOL SetDuration(int cFrames);
	BOOL SetDuration(int cFrames, int cMinFrames, int cMaxFrames);

public:
	BOOL PutFrame(const BYTE * pbFrame, int cbFrame, int nFrameSeq);
	int  GetFrame(LPBYTE pbFrame, int& cbFrame);
	int	 GetBufferingCount();

	BOOL GetLastOutputFrame(LPBYTE pbFrame, int& cbFrame);
	LPBYTE GetLastOutputFrame(int& cbFrame);

public:
	BYTE * BeginPutFrame(int cbFrame, int nFrameSeq);
	void EndPutFrame();

	int BeginGetFrame(const BYTE *& pbFrame, int& cbFrame);
	void EndGetFrame();

protected:
	BYTE * PutFrameImpl(int cbFrame, int nFrameSeq);
	BOOL PutFrameImpl(const BYTE * pbFrame, int cbFrame, int nFrameSeq);
	void RemoveHead();

	void UpdateStatistics(int nOperation);
	void CalculateJitter();

protected:
	typedef struct _FRAME {
		int		m_nType;
		int		m_cbFrame;
		LPBYTE	m_pbFrame;
	} FRAME, *PFRAME;

	static void SwapFrameBuffer(FRAME& frame1, FRAME& frame2);

	int		m_cbMaxFrame;
	int		m_cFrameCount;
	int		m_cBufferingDuration;
	PFRAME	m_pFrameRingBuffer;
	FRAME	m_frameLastOutput;
	LPBYTE	m_pbFrameBuffer;
	BOOL	m_bEmpty;
	int		m_nRingHead;
	int		m_nRingTail;
	int		m_nOrigin;
	int		m_nLastSeqNo;
	int		m_nBufferCount;

	int		m_cDefBufferingDuration;
	int		m_cMinBufferingDuration;
	int		m_cMaxBufferingDuration;
	int		m_nBufferingLevel;
	int		m_nLastBufferingLevel;
	int		m_nLastJitter;
	int		m_nMaxHistJitter;
	int		m_nStableHist;

	enum {
		OP_INIT,
		OP_GET,
		OP_PUT
	};
	int		m_nLastOperation;
};

