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

#ifndef _NVCODEC_H
#define _NVCODEC_H
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dynlink_cuda.h"
#include "nvUtils.h"
#include "nvEncodeAPI.h"

#if defined(NV_WINDOWS)
#include "d3d9.h"
#define NVENCAPI __stdcall
#pragma warning(disable : 4996)
#elif defined(NV_UNIX)
#include <dlfcn.h>
#include <string.h>
#define NVENCAPI
#endif

#define CODEC_VERSION "1.0"

#define DEFAULT_I_QFACTOR -0.8f
#define DEFAULT_B_QFACTOR 1.25f
#define DEFAULT_I_QOFFSET 0.f
#define DEFAULT_B_QOFFSET 1.25f

#define SET_VER(configStruct, type)                \
	{                                          \
		configStruct.version = type##_VER; \
	}
typedef NVENCSTATUS(NVENCAPI *MYPROC)(NV_ENCODE_API_FUNCTION_LIST *);

/*enum
{
	NV_ENC_H264 = 0,
	NV_ENC_HEVC = 1,
};*/

typedef struct _EncodeInputBuffer {
	unsigned int dwWidth;
	unsigned int dwHeight;
	IDirect3DSurface9 *pNV12Surface;
	CUdeviceptr pNV12devPtr;
	uint32_t uNV12Stride;
	CUdeviceptr pNV12TempdevPtr;
	uint32_t uNV12TempStride;
	void *nvRegisteredResource;
	NV_ENC_INPUT_PTR hInputSurface;
	NV_ENC_BUFFER_FORMAT bufferFmt;
} EncodeInputBuffer;

typedef struct _EncodeOutputBuffer {
	unsigned int dwBitstreamBufferSize;
	NV_ENC_OUTPUT_PTR hBitstreamBuffer;
	HANDLE hOutputEvent;
	bool bWaitOnEvent;
	bool bEOSFlag;
} EncodeOutputBuffer;

typedef struct _EncodeBuffer {
	EncodeOutputBuffer stOutputBfr;
	EncodeInputBuffer stInputBfr;
} EncodeBuffer;

typedef struct _NvEncPictureCommand {
	bool bResolutionChangePending;
	bool bBitrateChangePending;
	bool bForceIDR;
	bool bForceIntraRefresh;
	bool bInvalidateRefFrames;
	uint32_t newWidth;
	uint32_t newHeight;
	uint32_t newBitrate;
	uint32_t newVBVSize;
	uint32_t intraRefreshDuration;
	uint32_t numRefFramesToInvalidate;
	uint32_t refFrameNumbers[16];
} NvEncPictureCommand;

class IEncodedCallback {
public:
	virtual BOOL OnEncodedVideoCallback(BYTE *lpData, DWORD cbStride) = 0;
};

class CNvCodec {
public:
	CNvCodec();
	~CNvCodec();

public:
	//Lib API Functions
	//Doc 2
	NVENCSTATUS Initialize();

	//Setting up hardware for encoding
	//Doc 3.1 open an encoding session and initializing encode device
	NVENCSTATUS NvEncOpenEncodeSession(void *device, uint32_t deviceType);
	NVENCSTATUS NvEncOpenEncodeSessionEx(void *device,
					     NV_ENC_DEVICE_TYPE deviceType);
	//Doc 3.2 selecting encode codec guid
	NVENCSTATUS NvEncGetEncodeGUIDCount(uint32_t *encodeGUIDCount);
	NVENCSTATUS NvEncGetEncodeGUIDs(GUID *GUIDs, uint32_t guidArraySize,
					uint32_t *GUIDCount);
	//Doc 3.3 encoder preset configurations
	NVENCSTATUS NvEncGetEncodePresetCount(GUID encodeGUID,
					      uint32_t *encodePresetGUIDCount);
	NVENCSTATUS NvEncGetEncodePresetGUIDs(GUID encodeGUID,
					      GUID *presetGUIDs,
					      uint32_t guidArraySize,
					      uint32_t *encodePresetGUIDCount);
	NVENCSTATUS
	NvEncGetEncodePresetConfig(GUID encodeGUID, GUID presetGUID,
				   NV_ENC_PRESET_CONFIG *presetConfig);
	//Doc 3.5 get support list of input formats
	NVENCSTATUS NvEncGetInputFormatCount(GUID encodeGUID,
					     uint32_t *inputFmtCount);
	NVENCSTATUS NvEncGetInputFormats(GUID encodeGUID,
					 NV_ENC_BUFFER_FORMAT *inputFmts,
					 uint32_t inputFmtArraySize,
					 uint32_t *inputFmtCount);
	//Doc 3.6 querying capbility values
	NVENCSTATUS NvEncGetEncodeCaps(GUID encodeGUID,
				       NV_ENC_CAPS_PARAM *capsParam,
				       int *capsVal);
	//Doc 3.7/3.8 initializing encode session attributes and the hardware encoder session
	NVENCSTATUS
	NvEncInitializeEncoder(NV_ENC_INITIALIZE_PARAMS *stCreateEncodeParams);
	//Doc 3.9 create resources required to hold input/output data
	NVENCSTATUS NvEncCreateInputBuffer(uint32_t width, uint32_t height,
					   void **inputBuffer,
					   NV_ENC_BUFFER_FORMAT inputFormat);
	NVENCSTATUS NvEncCreateBitstreamBuffer(uint32_t size,
					       void **bitstreamBuffer);
	//Doc 3.10 retrieving sequence parameters
	NVENCSTATUS NvEncGetSequenceParams(
		NV_ENC_SEQUENCE_PARAM_PAYLOAD *sequenceParamPayload);

	//encoding the video stream
	//Doc 4.1 perparing input buffers for encoding
	NVENCSTATUS NvEncLockInputBuffer(void *inputBuffer,
					 void **bufferDataPtr, uint32_t *pitch);
	//Doc 4.2/4.3 configuring per-frame encode parameters and submitting input frame for encoding
	NVENCSTATUS
	NvEncEncodeFrame(EncodeBuffer *pEncodeBuffer,
			 NvEncPictureCommand *encPicCommand, uint32_t width,
			 uint32_t height, uint64_t encodeIndex, GUID codecGUID,
			 NV_ENC_PIC_STRUCT ePicStruct = NV_ENC_PIC_STRUCT_FRAME,
			 int8_t *qpDeltaMapArray = NULL,
			 uint32_t qpDeltaMapArraySize = 0);
	//Doc 4.4 retrieving encoded output
	NVENCSTATUS
	NvEncLockBitstream(NV_ENC_LOCK_BITSTREAM *lockBitstreamBufferParams);
	NVENCSTATUS NvEncUnlockBitstream(NV_ENC_OUTPUT_PTR bitstreamBuffer);

	//end of encoding
	//Doc 5.1 notifying the end of input stream
	//Doc 5.2 releasing resources
	NVENCSTATUS NvEncUnlockInputBuffer(NV_ENC_INPUT_PTR inputBuffer);
	NVENCSTATUS NvEncDestroyInputBuffer(NV_ENC_INPUT_PTR inputBuffer);
	NVENCSTATUS
	NvEncDestroyBitstreamBuffer(NV_ENC_OUTPUT_PTR bitstreamBuffer);
	//Doc 5.3 closing encode session
	NVENCSTATUS NvEncDestroyEncoder();

	//Others
	NVENCSTATUS NvEncRegisterAsyncEvent(void **completionEvent);
	NVENCSTATUS NvEncUnregisterAsyncEvent(void *completionEvent);
	NVENCSTATUS NvEncFlushEncoderQueue(void *hEOSEvent);
	NVENCSTATUS ProcessOutput(const EncodeBuffer *pEncodeBuffer,
				  IEncodedCallback *pCallback);

private:
	//lib handle
	HINSTANCE m_hinstLib;
	NV_ENCODE_API_FUNCTION_LIST *m_pEncodeAPI;
	void *m_hEncoder;
};

#endif