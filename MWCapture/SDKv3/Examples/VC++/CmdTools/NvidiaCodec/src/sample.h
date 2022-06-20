/************************************************************************************************/
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

#ifndef _SAMPLE_H
#define _SAMPLE_H
#include <new>
#include "NvCodec.h"
#include "capturethread.h"

#define CONST_QP 28
#define MAX_ENCODE_QUEUE 32
#define BITSTREAM_BUFFER_SIZE 2 * 1024 * 1024


//Queue
template<class T>
class CNvQueue {
	T** m_pBuffer;
	unsigned int m_uSize;
	unsigned int m_uPendingCount;
	unsigned int m_uAvailableIdx;
	unsigned int m_uPendingndex;
public:
	CNvQueue() : m_pBuffer(NULL), m_uSize(0), m_uPendingCount(0), m_uAvailableIdx(0),
		m_uPendingndex(0)
	{
	}

	~CNvQueue()
	{
		delete[] m_pBuffer;
	}

	bool Initialize(T *pItems, unsigned int uSize)
	{
		m_uSize = uSize;
		m_uPendingCount = 0;
		m_uAvailableIdx = 0;
		m_uPendingndex = 0;
		m_pBuffer = new T *[m_uSize];
		for (unsigned int i = 0; i < m_uSize; i++)
		{
			m_pBuffer[i] = &pItems[i];
		}
		return true;
	}


	T * GetAvailable()
	{
		T *pItem = NULL;
		if (m_uPendingCount == m_uSize)
		{
			return NULL;
		}
		pItem = m_pBuffer[m_uAvailableIdx];
		m_uAvailableIdx = (m_uAvailableIdx + 1) % m_uSize;
		m_uPendingCount += 1;
		return pItem;
	}

	T* GetPending()
	{
		if (m_uPendingCount == 0)
		{
			return NULL;
		}

		T *pItem = m_pBuffer[m_uPendingndex];
		m_uPendingndex = (m_uPendingndex + 1) % m_uSize;
		m_uPendingCount -= 1;
		return pItem;
	}
};


//Params
typedef struct _InputParams
{
	int				width;
	int				height;
	float			fps;
	int				bitrate;
	int				numB;
	GUID			presetGUID;
	int				framesToEncode;
	BOOL			bPushInfo;
} InputParams;


typedef struct _EncodeFrameConfig
{
	EncodeBuffer *pEncodeBuffer;
	BYTE *pInputSurface;
	uint32_t stride[3];
	uint32_t width;
	uint32_t height;
	uint32_t lockedPitch;
} EncodeFrameConfig;


class CSample :public ICaptureCallback
{
public:
	CSample();
	~CSample();

public:
	BOOL InitEncode(IEncodedCallback *pCallback);
	BOOL CreateEncode(InputParams *params);

	BOOL QueryEncodeEndStatus();
	double QueryProcessTime();

	BOOL FlushEncoder();
	void Exit();

	//Capture callback
	virtual BYTE* Lock();
	virtual BOOL UnLock();

protected:
	BOOL InitCuda(uint32_t deviceID);
	BOOL AllocBuffers();
	void ReleaseBuffers();
	BOOL ValidateEncodeGUID(GUID inputCodecGUID);
	BOOL ValidatePresetGUID(GUID inputPresetGuid, GUID inputCodecGuid);

private:
	//handle
	void		*m_pDevice;
	CNvCodec	*m_pNvCodec;

	//encode params
	NV_ENC_INITIALIZE_PARAMS  m_stCreateEncodeParams;
	NV_ENC_CONFIG	m_stEncodeConfig;
	//input/output
	CNvQueue<EncodeBuffer>	m_EncodeBufferQueue;
	EncodeBuffer	m_stEncodeBuffer[MAX_ENCODE_QUEUE];
	uint32_t m_uEncodeBufferCount;
	EncodeOutputBuffer m_stEOSOutputBfr;
	EncodeFrameConfig m_stEncodeFrame;

	IEncodedCallback *m_pCallback;
	BOOL m_bEncoderInitialized;
	int		m_nBFrameNumber;
	BOOL	m_bPushInfo;
	//Counter
	uint64_t m_nFramesIntoEncode;		//into encode
	uint64_t m_nFramesOutEncode;		//out encode
	uint64_t m_nFramesFlushOutEncode;	//flush out encode
	uint64_t m_nFramesToEncode;
	BOOL	m_bFinished;
	//time
	unsigned long long m_llStartTemp;
	unsigned long long m_llEndTemp;
	double m_llTotalProcessTime;
};


#endif