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

#include "nvcodec.h"

CNvCodec::CNvCodec() {}

CNvCodec::~CNvCodec() {}

/*****************************************
* API Functions
******************************************/
NVENCSTATUS CNvCodec::Initialize()
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	MYPROC nvEncodeAPICreateInstance;

#if defined(_WIN64)
	m_hinstLib = LoadLibrary(TEXT("nvEncodeAPI64.dll")); //64bits
#else
	m_hinstLib = LoadLibrary(TEXT("nvEncodeAPI.dll")); //32bits
#endif
	if (m_hinstLib == NULL)
		return NV_ENC_ERR_OUT_OF_MEMORY;

	nvEncodeAPICreateInstance =
		(MYPROC)GetProcAddress(m_hinstLib, "NvEncodeAPICreateInstance");
	if (nvEncodeAPICreateInstance == NULL)
		return NV_ENC_ERR_OUT_OF_MEMORY;

	m_pEncodeAPI = new NV_ENCODE_API_FUNCTION_LIST;
	if (m_pEncodeAPI == NULL)
		return NV_ENC_ERR_OUT_OF_MEMORY;

	memset(m_pEncodeAPI, 0, sizeof(NV_ENCODE_API_FUNCTION_LIST));
	m_pEncodeAPI->version = NV_ENCODE_API_FUNCTION_LIST_VER;
	nvStatus = nvEncodeAPICreateInstance(m_pEncodeAPI);
	if (nvStatus != NV_ENC_SUCCESS)
		return nvStatus;

	return NV_ENC_SUCCESS;
}

NVENCSTATUS CNvCodec::NvEncOpenEncodeSession(void *device, uint32_t deviceType)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncOpenEncodeSession(device, deviceType,
							&m_hEncoder);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncOpenEncodeSessionEx(void *device,
					       NV_ENC_DEVICE_TYPE deviceType)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS openSessionExParams;

	memset(&openSessionExParams, 0, sizeof(openSessionExParams));
	SET_VER(openSessionExParams, NV_ENC_OPEN_ENCODE_SESSION_EX_PARAMS);

	openSessionExParams.device = device;
	openSessionExParams.deviceType = deviceType;
	openSessionExParams.apiVersion = NVENCAPI_VERSION;

	nvStatus = m_pEncodeAPI->nvEncOpenEncodeSessionEx(&openSessionExParams,
							  &m_hEncoder);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetEncodeGUIDCount(uint32_t *encodeGUIDCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDCount(m_hEncoder,
							 encodeGUIDCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetEncodeGUIDs(GUID *GUIDs, uint32_t guidArraySize,
					  uint32_t *GUIDCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodeGUIDs(m_hEncoder, GUIDs,
						     guidArraySize, GUIDCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetEncodePresetCount(GUID encodeGUID,
						uint32_t *encodePresetGUIDCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodePresetCount(
		m_hEncoder, encodeGUID, encodePresetGUIDCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetEncodePresetGUIDs(GUID encodeGUID,
						GUID *presetGUIDs,
						uint32_t guidArraySize,
						uint32_t *encodePresetGUIDCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodePresetGUIDs(
		m_hEncoder, encodeGUID, presetGUIDs, guidArraySize,
		encodePresetGUIDCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS
CNvCodec::NvEncGetEncodePresetConfig(GUID encodeGUID, GUID presetGUID,
				     NV_ENC_PRESET_CONFIG *presetConfig)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodePresetConfig(
		m_hEncoder, encodeGUID, presetGUID, presetConfig);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetInputFormatCount(GUID encodeGUID,
					       uint32_t *inputFmtCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetInputFormatCount(
		m_hEncoder, encodeGUID, inputFmtCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetInputFormats(GUID encodeGUID,
					   NV_ENC_BUFFER_FORMAT *inputFmts,
					   uint32_t inputFmtArraySize,
					   uint32_t *inputFmtCount)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetInputFormats(m_hEncoder, encodeGUID,
						      inputFmts,
						      inputFmtArraySize,
						      inputFmtCount);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetEncodeCaps(GUID encodeGUID,
					 NV_ENC_CAPS_PARAM *capsParam,
					 int *capsVal)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetEncodeCaps(m_hEncoder, encodeGUID,
						    capsParam, capsVal);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS
CNvCodec::NvEncInitializeEncoder(NV_ENC_INITIALIZE_PARAMS *stCreateEncodeParams)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncInitializeEncoder(m_hEncoder,
							stCreateEncodeParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncCreateInputBuffer(uint32_t width, uint32_t height,
					     void **inputBuffer,
					     NV_ENC_BUFFER_FORMAT inputFormat)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_CREATE_INPUT_BUFFER createInputBufferParams;

	memset(&createInputBufferParams, 0, sizeof(createInputBufferParams));
	SET_VER(createInputBufferParams, NV_ENC_CREATE_INPUT_BUFFER);

	createInputBufferParams.width = width;
	createInputBufferParams.height = height;
	createInputBufferParams.memoryHeap = NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;
	createInputBufferParams.bufferFmt = inputFormat;

	nvStatus = m_pEncodeAPI->nvEncCreateInputBuffer(
		m_hEncoder, &createInputBufferParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	*inputBuffer = createInputBufferParams.inputBuffer;

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncCreateBitstreamBuffer(uint32_t size,
						 void **bitstreamBuffer)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_CREATE_BITSTREAM_BUFFER createBitstreamBufferParams;

	memset(&createBitstreamBufferParams, 0,
	       sizeof(createBitstreamBufferParams));
	SET_VER(createBitstreamBufferParams, NV_ENC_CREATE_BITSTREAM_BUFFER);

	createBitstreamBufferParams.size = size;
	createBitstreamBufferParams.memoryHeap =
		NV_ENC_MEMORY_HEAP_SYSMEM_CACHED;

	nvStatus = m_pEncodeAPI->nvEncCreateBitstreamBuffer(
		m_hEncoder, &createBitstreamBufferParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	*bitstreamBuffer = createBitstreamBufferParams.bitstreamBuffer;

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncGetSequenceParams(
	NV_ENC_SEQUENCE_PARAM_PAYLOAD *sequenceParamPayload)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncGetSequenceParams(m_hEncoder,
							sequenceParamPayload);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncLockInputBuffer(void *inputBuffer,
					   void **bufferDataPtr,
					   uint32_t *pitch)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_LOCK_INPUT_BUFFER lockInputBufferParams;

	memset(&lockInputBufferParams, 0, sizeof(lockInputBufferParams));
	SET_VER(lockInputBufferParams, NV_ENC_LOCK_INPUT_BUFFER);

	lockInputBufferParams.inputBuffer = inputBuffer;
	nvStatus = m_pEncodeAPI->nvEncLockInputBuffer(m_hEncoder,
						      &lockInputBufferParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	*bufferDataPtr = lockInputBufferParams.bufferDataPtr;
	*pitch = lockInputBufferParams.pitch;

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncEncodeFrame(EncodeBuffer *pEncodeBuffer,
				       NvEncPictureCommand *encPicCommand,
				       uint32_t width, uint32_t height,
				       uint64_t encodeIndex, GUID codecGUID,
				       NV_ENC_PIC_STRUCT ePicStruct,
				       int8_t *qpDeltaMapArray,
				       uint32_t qpDeltaMapArraySize)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_PIC_PARAMS encPicParams;

	memset(&encPicParams, 0, sizeof(encPicParams));
	SET_VER(encPicParams, NV_ENC_PIC_PARAMS);

	encPicParams.inputBuffer = pEncodeBuffer->stInputBfr.hInputSurface;
	encPicParams.bufferFmt = pEncodeBuffer->stInputBfr.bufferFmt;
	encPicParams.inputWidth = width;
	encPicParams.inputHeight = height;
	encPicParams.outputBitstream =
		pEncodeBuffer->stOutputBfr.hBitstreamBuffer;
	encPicParams.completionEvent = pEncodeBuffer->stOutputBfr.hOutputEvent;
	encPicParams.inputTimeStamp = encodeIndex;
	encPicParams.pictureStruct = ePicStruct;
	encPicParams.qpDeltaMap = qpDeltaMapArray;
	encPicParams.qpDeltaMapSize = qpDeltaMapArraySize;

	if (encPicCommand) {
		if (encPicCommand->bForceIDR) {
			encPicParams.encodePicFlags |= NV_ENC_PIC_FLAG_FORCEIDR;
		}

		if (encPicCommand->bForceIntraRefresh) {
			if (codecGUID == NV_ENC_CODEC_HEVC_GUID) {
				encPicParams.codecPicParams.hevcPicParams
					.forceIntraRefreshWithFrameCnt =
					encPicCommand->intraRefreshDuration;
			} else {
				encPicParams.codecPicParams.h264PicParams
					.forceIntraRefreshWithFrameCnt =
					encPicCommand->intraRefreshDuration;
			}
		}
	}

	nvStatus = m_pEncodeAPI->nvEncEncodePicture(m_hEncoder, &encPicParams);
	if (nvStatus != NV_ENC_SUCCESS &&
	    nvStatus != NV_ENC_ERR_NEED_MORE_INPUT) {
		assert(0);
		return nvStatus;
	}

	return NV_ENC_SUCCESS;
}

NVENCSTATUS
CNvCodec::NvEncLockBitstream(NV_ENC_LOCK_BITSTREAM *lockBitstreamBufferParams)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncLockBitstream(m_hEncoder,
						    lockBitstreamBufferParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncUnlockBitstream(NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus =
		m_pEncodeAPI->nvEncUnlockBitstream(m_hEncoder, bitstreamBuffer);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncUnlockInputBuffer(NV_ENC_INPUT_PTR inputBuffer)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus =
		m_pEncodeAPI->nvEncUnlockInputBuffer(m_hEncoder, inputBuffer);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncDestroyInputBuffer(NV_ENC_INPUT_PTR inputBuffer)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	if (inputBuffer) {
		nvStatus = m_pEncodeAPI->nvEncDestroyInputBuffer(m_hEncoder,
								 inputBuffer);
		if (nvStatus != NV_ENC_SUCCESS) {
			assert(0);
		}
	}

	return nvStatus;
}

NVENCSTATUS
CNvCodec::NvEncDestroyBitstreamBuffer(NV_ENC_OUTPUT_PTR bitstreamBuffer)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	if (bitstreamBuffer) {
		nvStatus = m_pEncodeAPI->nvEncDestroyBitstreamBuffer(
			m_hEncoder, bitstreamBuffer);
		if (nvStatus != NV_ENC_SUCCESS) {
			assert(0);
		}
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncDestroyEncoder()
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pEncodeAPI->nvEncDestroyEncoder(m_hEncoder);

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncRegisterAsyncEvent(void **completionEvent)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_EVENT_PARAMS eventParams;

	memset(&eventParams, 0, sizeof(eventParams));
	SET_VER(eventParams, NV_ENC_EVENT_PARAMS);

#if defined(NV_WINDOWS)
	eventParams.completionEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
	eventParams.completionEvent = NULL;
#endif
	nvStatus =
		m_pEncodeAPI->nvEncRegisterAsyncEvent(m_hEncoder, &eventParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}

	*completionEvent = eventParams.completionEvent;

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncUnregisterAsyncEvent(void *completionEvent)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_EVENT_PARAMS eventParams;

	if (completionEvent) {
		memset(&eventParams, 0, sizeof(eventParams));
		SET_VER(eventParams, NV_ENC_EVENT_PARAMS);

		eventParams.completionEvent = completionEvent;

		nvStatus = m_pEncodeAPI->nvEncUnregisterAsyncEvent(
			m_hEncoder, &eventParams);
		if (nvStatus != NV_ENC_SUCCESS) {
			assert(0);
		}
	}

	return nvStatus;
}

NVENCSTATUS CNvCodec::NvEncFlushEncoderQueue(void *hEOSEvent)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	NV_ENC_PIC_PARAMS encPicParams;
	memset(&encPicParams, 0, sizeof(encPicParams));
	SET_VER(encPicParams, NV_ENC_PIC_PARAMS);
	encPicParams.encodePicFlags = NV_ENC_PIC_FLAG_EOS;
	encPicParams.completionEvent = hEOSEvent;
	nvStatus = m_pEncodeAPI->nvEncEncodePicture(m_hEncoder, &encPicParams);
	if (nvStatus != NV_ENC_SUCCESS) {
		assert(0);
	}
	return nvStatus;
}

NVENCSTATUS CNvCodec::ProcessOutput(const EncodeBuffer *pEncodeBuffer,
				    IEncodedCallback *pCallback)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	if (pEncodeBuffer->stOutputBfr.hBitstreamBuffer == NULL &&
	    pEncodeBuffer->stOutputBfr.bEOSFlag == FALSE) {
		return NV_ENC_ERR_INVALID_PARAM;
	}

	if (pEncodeBuffer->stOutputBfr.bWaitOnEvent == TRUE) {
		if (!pEncodeBuffer->stOutputBfr.hOutputEvent) {
			return NV_ENC_ERR_INVALID_PARAM;
		}
#if defined(NV_WINDOWS)
		WaitForSingleObject(pEncodeBuffer->stOutputBfr.hOutputEvent,
				    INFINITE);
#endif
	}

	if (pEncodeBuffer->stOutputBfr.bEOSFlag)
		return NV_ENC_SUCCESS;

	nvStatus = NV_ENC_SUCCESS;
	NV_ENC_LOCK_BITSTREAM lockBitstreamData;
	memset(&lockBitstreamData, 0, sizeof(lockBitstreamData));
	SET_VER(lockBitstreamData, NV_ENC_LOCK_BITSTREAM);
	lockBitstreamData.outputBitstream =
		pEncodeBuffer->stOutputBfr.hBitstreamBuffer;
	lockBitstreamData.doNotWait = false;

	nvStatus = m_pEncodeAPI->nvEncLockBitstream(m_hEncoder,
						    &lockBitstreamData);
	if (nvStatus == NV_ENC_SUCCESS) {
		//fwrite(lockBitstreamData.bitstreamBufferPtr, 1, lockBitstreamData.bitstreamSizeInBytes, m_fOutput);
		if (pCallback != NULL)
			pCallback->OnEncodedVideoCallback(
				(BYTE *)lockBitstreamData.bitstreamBufferPtr,
				lockBitstreamData.bitstreamSizeInBytes);
		nvStatus = m_pEncodeAPI->nvEncUnlockBitstream(
			m_hEncoder,
			pEncodeBuffer->stOutputBfr.hBitstreamBuffer);
	} else {
		PRINTERR("lock bitstream function failed \n");
	}

	return nvStatus;
}