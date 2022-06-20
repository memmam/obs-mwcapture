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

#include "sample.h"

CSample::CSample()
{
	m_pNvCodec = new CNvCodec;

	memset(&m_stCreateEncodeParams, 0, sizeof(m_stCreateEncodeParams));
	SET_VER(m_stCreateEncodeParams, NV_ENC_INITIALIZE_PARAMS);

	memset(&m_stEncodeConfig, 0, sizeof(m_stEncodeConfig));
	SET_VER(m_stEncodeConfig, NV_ENC_CONFIG);

	for (int i = 0; i < MAX_ENCODE_QUEUE; i++) {
		memset(&m_stEncodeBuffer[i], 0, sizeof(EncodeBuffer));
	}

	m_nBFrameNumber = 0;
	m_bPushInfo = FALSE;
	m_bEncoderInitialized = FALSE;
	m_nFramesIntoEncode = 0;
	m_nFramesOutEncode = 0;
	m_nFramesFlushOutEncode = 0;
	m_nFramesToEncode = 0;
	m_bFinished = FALSE;
	m_llStartTemp = 0;
	m_llEndTemp = 0;
	m_llTotalProcessTime = 0;
	m_pCallback = NULL;
}

CSample::~CSample()
{
	delete m_pNvCodec;
	m_pNvCodec = NULL;
}

BOOL CSample::InitCuda(uint32_t deviceID)
{
	CUresult cuResult;
	CUdevice device;
	CUcontext cuContextCurr;
	int deviceCount = 0;
	int SMminor = 0, SMmajor = 0;
	typedef HMODULE CUDADRIVER;

	CUDADRIVER hHandleDriver = 0;
	cuResult = cuInit(0, __CUDA_API_VERSION, hHandleDriver);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	cuResult = cuDeviceGetCount(&deviceCount);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	// If dev is negative value, we clamp to 0
	if (deviceID < 0)
		deviceID = 0;
	if (deviceID > (unsigned int)deviceCount - 1)
		return FALSE;

	cuResult = cuDeviceGet(&device, deviceID);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	cuResult = cuDeviceComputeCapability(&SMmajor, &SMminor, deviceID);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	if (((SMmajor << 4) + SMminor) < 0x30)
		return FALSE;

	cuResult = cuCtxCreate((CUcontext *)(&m_pDevice), 0, device);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	cuResult = cuCtxPopCurrent(&cuContextCurr);
	if (cuResult != CUDA_SUCCESS)
		return FALSE;

	return TRUE;
}

BOOL CSample::InitEncode(IEncodedCallback *pCallback)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	uint32_t deviceID = 0;
	if (!InitCuda(deviceID))
		return FALSE;

	m_pNvCodec->Initialize();
	nvStatus = m_pNvCodec->NvEncOpenEncodeSessionEx(
		m_pDevice, NV_ENC_DEVICE_TYPE_CUDA);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	m_pCallback = pCallback;
	return TRUE;
}

BOOL CSample::CreateEncode(InputParams *params)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	m_nBFrameNumber = params->numB;
	m_bPushInfo = params->bPushInfo;
	m_nFramesToEncode = params->framesToEncode;

	m_stCreateEncodeParams.encodeGUID = NV_ENC_CODEC_H264_GUID;
	m_stCreateEncodeParams.presetGUID = params->presetGUID;
	m_stCreateEncodeParams.encodeConfig = &m_stEncodeConfig;
	m_stCreateEncodeParams.encodeWidth = params->width;
	m_stCreateEncodeParams.encodeHeight = params->height; //Resolution
	m_stCreateEncodeParams.darWidth = m_stCreateEncodeParams.encodeWidth;
	m_stCreateEncodeParams.darHeight = m_stCreateEncodeParams.encodeHeight;
	m_stCreateEncodeParams.maxEncodeWidth =
		m_stCreateEncodeParams.encodeWidth;
	m_stCreateEncodeParams.maxEncodeHeight =
		m_stCreateEncodeParams.encodeWidth;
	m_stCreateEncodeParams.frameRateNum = params->fps; //fps
	m_stCreateEncodeParams.frameRateDen = 1;
	m_stCreateEncodeParams.enableEncodeAsync = 0; //Async mode
	m_stCreateEncodeParams.enablePTD = 1;
	m_stCreateEncodeParams.reportSliceOffsets = 0;
	m_stCreateEncodeParams.enableSubFrameWrite = 0;

	if (!ValidateEncodeGUID(m_stCreateEncodeParams.encodeGUID))
		return FALSE;

	if (!ValidatePresetGUID(m_stCreateEncodeParams.presetGUID,
				m_stCreateEncodeParams.encodeGUID))
		return FALSE;

	//Configure
	NV_ENC_PRESET_CONFIG stPresetCfg;
	memset(&stPresetCfg, 0, sizeof(NV_ENC_PRESET_CONFIG));
	SET_VER(stPresetCfg, NV_ENC_PRESET_CONFIG);
	SET_VER(stPresetCfg.presetCfg, NV_ENC_CONFIG);
	nvStatus = m_pNvCodec->NvEncGetEncodePresetConfig(
		m_stCreateEncodeParams.encodeGUID,
		m_stCreateEncodeParams.presetGUID, &stPresetCfg);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;
	memcpy(&m_stEncodeConfig, &stPresetCfg.presetCfg,
	       sizeof(NV_ENC_CONFIG));

	m_stEncodeConfig.gopLength = NVENC_INFINITE_GOPLENGTH; //GOP Size
	m_stEncodeConfig.frameIntervalP =
		params->numB +
		1; //Distance between I/P frame. 0+1 means no B-Frame
	m_stEncodeConfig.frameFieldMode =
		NV_ENC_PARAMS_FRAME_FIELD_MODE_FRAME; //Progressive frame
	m_stEncodeConfig.mvPrecision = NV_ENC_MV_PRECISION_QUARTER_PEL;
	//Bitrate control,if user set bitrate
	m_stEncodeConfig.rcParams.rateControlMode = NV_ENC_PARAMS_RC_CONSTQP;
	m_stEncodeConfig.rcParams.averageBitRate = params->bitrate; //bitRate
	m_stEncodeConfig.rcParams.maxBitRate =
		m_stEncodeConfig.rcParams.averageBitRate;
	m_stEncodeConfig.rcParams.vbvBufferSize = 0;
	m_stEncodeConfig.rcParams.vbvInitialDelay = 0;
	if (m_stEncodeConfig.rcParams.rateControlMode ==
	    NV_ENC_PARAMS_RC_CONSTQP) {
		m_stEncodeConfig.rcParams.constQP.qpInterP =
			m_stCreateEncodeParams.presetGUID ==
					NV_ENC_PRESET_LOSSLESS_HP_GUID
				? 0
				: CONST_QP;
		m_stEncodeConfig.rcParams.constQP.qpInterB =
			m_stCreateEncodeParams.presetGUID ==
					NV_ENC_PRESET_LOSSLESS_HP_GUID
				? 0
				: CONST_QP;
		m_stEncodeConfig.rcParams.constQP.qpIntra =
			m_stCreateEncodeParams.presetGUID ==
					NV_ENC_PRESET_LOSSLESS_HP_GUID
				? 0
				: CONST_QP;
	}
	//Other bitRate Control method
	if (m_stEncodeConfig.rcParams.rateControlMode == NV_ENC_PARAMS_RC_VBR ||
	    m_stEncodeConfig.rcParams.rateControlMode ==
		    NV_ENC_PARAMS_RC_VBR_MINQP ||
	    m_stEncodeConfig.rcParams.rateControlMode ==
		    NV_ENC_PARAMS_RC_2_PASS_VBR) {
		m_stEncodeConfig.rcParams.enableInitialRCQP = 1;
		m_stEncodeConfig.rcParams.initialRCQP.qpInterP = CONST_QP;
		if (DEFAULT_I_QFACTOR != 0.0 && DEFAULT_B_QFACTOR != 0.0) {
			m_stEncodeConfig.rcParams.initialRCQP.qpIntra =
				(int)(CONST_QP * FABS(DEFAULT_I_QFACTOR) +
				      DEFAULT_I_QOFFSET);
			m_stEncodeConfig.rcParams.initialRCQP.qpInterB =
				(int)(CONST_QP * FABS(DEFAULT_B_QFACTOR) +
				      DEFAULT_B_QOFFSET);
		} else {
			m_stEncodeConfig.rcParams.initialRCQP.qpIntra =
				CONST_QP;
			m_stEncodeConfig.rcParams.initialRCQP.qpInterB =
				CONST_QP;
		}
	}
	//FormatIDC and IDR
	if (m_stCreateEncodeParams.encodeGUID == NV_ENC_CODEC_H264_GUID) {
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.chromaFormatIDC =
			1;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.idrPeriod =
			m_stEncodeConfig.gopLength;
	} else {
		m_stEncodeConfig.encodeCodecConfig.h264Config.chromaFormatIDC =
			1;
		m_stEncodeConfig.encodeCodecConfig.h264Config.idrPeriod =
			m_stEncodeConfig.gopLength;
	}
	//intraRefreshEnableFlag
	/*if (m_stCreateEncodeParams.encodeGUID == NV_ENC_CODEC_HEVC_GUID)
	{
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.enableIntraRefresh = 1;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.intraRefreshPeriod = 0;
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.intraRefreshCnt = 0;
	}
	else
	{
		m_stEncodeConfig.encodeCodecConfig.h264Config.enableIntraRefresh = 1;
		m_stEncodeConfig.encodeCodecConfig.h264Config.intraRefreshPeriod = 0;
		m_stEncodeConfig.encodeCodecConfig.h264Config.intraRefreshCnt = 0;
	}*/
	//invalidateRefFramesEnableFlag
	/*if (m_stCreateEncodeParams.encodeGUID == NV_ENC_CODEC_HEVC_GUID)
	{
		m_stEncodeConfig.encodeCodecConfig.hevcConfig.maxNumRefFramesInDPB = 16;
	}
	else
	{
		m_stEncodeConfig.encodeCodecConfig.h264Config.maxNumRefFrames = 16;
	}*/
	//qpDeltaMapFile
	//m_stEncodeConfig.rcParams.enableExtQPDeltaMap = 1;

	//Async mode support
	NV_ENC_CAPS_PARAM stCapsParam;
	int asyncMode = 0;
	memset(&stCapsParam, 0, sizeof(NV_ENC_CAPS_PARAM));
	SET_VER(stCapsParam, NV_ENC_CAPS_PARAM);

	stCapsParam.capsToQuery = NV_ENC_CAPS_ASYNC_ENCODE_SUPPORT;
	m_pNvCodec->NvEncGetEncodeCaps(m_stCreateEncodeParams.encodeGUID,
				       &stCapsParam, &asyncMode);
	m_stCreateEncodeParams.enableEncodeAsync = asyncMode;
	//enableTemporalAQ
	/*NV_ENC_CAPS_PARAM stCapsParam;
	memset(&stCapsParam, 0, sizeof(NV_ENC_CAPS_PARAM));
	SET_VER(stCapsParam, NV_ENC_CAPS_PARAM);
	stCapsParam.capsToQuery = NV_ENC_CAPS_SUPPORT_TEMPORAL_AQ;
	int temporalAQSupported = 0;
	nvStatus = m_pNvCodec->NvEncGetEncodeCaps(m_stCreateEncodeParams.encodeGUID, &stCapsParam, &temporalAQSupported);
	if (nvStatus != NV_ENC_SUCCESS)
	{
		return FALSE;
	}
	else
	{
		if (temporalAQSupported == 1)
		{
			m_stEncodeConfig.rcParams.enableTemporalAQ = 1;
		}
		else
		{
			return FALSE;
		}
	}*/

	//Init Encoder
	nvStatus = m_pNvCodec->NvEncInitializeEncoder(&m_stCreateEncodeParams);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	if (!AllocBuffers())
		return FALSE;

	m_bEncoderInitialized = TRUE;
	return TRUE;
}

BOOL CSample::AllocBuffers()
{
	//如果有B帧，BufferCount = B帧数目 + 4;//min buffers is numb + 1 + 3 pipelining
	//如果没有B帧
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	int numMBs = ((m_stCreateEncodeParams.encodeWidth + 15) >> 4) *
		     ((m_stCreateEncodeParams.encodeHeight + 15) >> 4);
	int NumIOBuffers;

	if (numMBs >= 32768) //4kx2k
		NumIOBuffers = MAX_ENCODE_QUEUE / 8;
	else if (numMBs >= 16384) // 2kx2k
		NumIOBuffers = MAX_ENCODE_QUEUE / 4;
	else if (numMBs >= 8160) // 1920x1080
		NumIOBuffers = MAX_ENCODE_QUEUE / 2;
	else
		NumIOBuffers = MAX_ENCODE_QUEUE;

	m_uEncodeBufferCount = NumIOBuffers;

	m_EncodeBufferQueue.Initialize(m_stEncodeBuffer, m_uEncodeBufferCount);
	for (uint32_t i = 0; i < m_uEncodeBufferCount; i++) {
		nvStatus = m_pNvCodec->NvEncCreateInputBuffer(
			m_stCreateEncodeParams.encodeWidth,
			m_stCreateEncodeParams.encodeHeight,
			&m_stEncodeBuffer[i].stInputBfr.hInputSurface,
			NV_ENC_BUFFER_FORMAT_NV12);
		if (nvStatus != NV_ENC_SUCCESS)
			return FALSE;

		m_stEncodeBuffer[i].stInputBfr.bufferFmt =
			NV_ENC_BUFFER_FORMAT_NV12;
		m_stEncodeBuffer[i].stInputBfr.dwWidth =
			m_stCreateEncodeParams.encodeWidth;
		m_stEncodeBuffer[i].stInputBfr.dwHeight =
			m_stCreateEncodeParams.encodeHeight;
		nvStatus = m_pNvCodec->NvEncCreateBitstreamBuffer(
			BITSTREAM_BUFFER_SIZE,
			&m_stEncodeBuffer[i].stOutputBfr.hBitstreamBuffer);
		if (nvStatus != NV_ENC_SUCCESS)
			return FALSE;
		m_stEncodeBuffer[i].stOutputBfr.dwBitstreamBufferSize =
			BITSTREAM_BUFFER_SIZE;
		if (m_stCreateEncodeParams.enableEncodeAsync) {
			nvStatus = m_pNvCodec->NvEncRegisterAsyncEvent(
				&m_stEncodeBuffer[i].stOutputBfr.hOutputEvent);
			if (nvStatus != NV_ENC_SUCCESS)
				return FALSE;
			m_stEncodeBuffer[i].stOutputBfr.bWaitOnEvent = true;
		} else
			m_stEncodeBuffer[i].stOutputBfr.hOutputEvent = NULL;
	}

	m_stEOSOutputBfr.bEOSFlag = TRUE;
	if (m_stCreateEncodeParams.enableEncodeAsync) {
		nvStatus = m_pNvCodec->NvEncRegisterAsyncEvent(
			&m_stEOSOutputBfr.hOutputEvent);
		if (nvStatus != NV_ENC_SUCCESS)
			return FALSE;
	} else
		m_stEOSOutputBfr.hOutputEvent = NULL;

	return TRUE;
}

BYTE *CSample::Lock()
{
	if (!m_bEncoderInitialized)
		return NULL;

	if (m_bFinished)
		return NULL;

	//time
	if (m_nFramesIntoEncode == 0) //start
	{
		unsigned long long lFreqTemp;

		NvQueryPerformanceCounter(&m_llStartTemp);
		NvQueryPerformanceFrequency(&lFreqTemp);
	} else {
		if (m_nFramesIntoEncode >= m_nFramesToEncode) //end
		{
			unsigned long long lFreqTemp;

			printf("\n");
			NvQueryPerformanceCounter(&m_llEndTemp);
			NvQueryPerformanceFrequency(&lFreqTemp);
			double elapsedTimeTemp =
				(double)(m_llEndTemp - m_llStartTemp);
			m_llTotalProcessTime =
				(elapsedTimeTemp * 1000.0) / lFreqTemp; //ms
			m_bFinished = TRUE;
			return NULL;
		} else //normal count
		{
			unsigned long long lFreqTemp;
			unsigned long long lCurTemp;
			double llCurrentTotal;

			NvQueryPerformanceCounter(&lCurTemp);
			NvQueryPerformanceFrequency(&lFreqTemp);
			double elapsedTimeTemp =
				(double)(lCurTemp - m_llStartTemp);
			llCurrentTotal =
				(elapsedTimeTemp * 1000.0) / lFreqTemp; //ms
			if ((!m_bPushInfo) && (m_nFramesIntoEncode % 10 == 0))
				printf("\r  %.2lf ms passed , %lld frames has been encoded , avg encode fps: %.2lf",
				       llCurrentTotal, m_nFramesIntoEncode,
				       m_nFramesIntoEncode * 1000 /
					       llCurrentTotal);
		}
	}

	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	memset(&m_stEncodeFrame, 0, sizeof(m_stEncodeFrame));
	m_stEncodeFrame.stride[0] = m_stCreateEncodeParams.encodeWidth;
	m_stEncodeFrame.stride[1] = m_stEncodeFrame.stride[2] =
		(m_stEncodeFrame.stride[0] / 2);
	m_stEncodeFrame.width = m_stCreateEncodeParams.encodeWidth;
	m_stEncodeFrame.height = m_stCreateEncodeParams.encodeHeight;
	m_stEncodeFrame.pEncodeBuffer = m_EncodeBufferQueue.GetAvailable();
	if (!m_stEncodeFrame.pEncodeBuffer) {
		m_nFramesOutEncode++;
		m_pNvCodec->ProcessOutput(m_EncodeBufferQueue.GetPending(),
					  m_pCallback);
		m_stEncodeFrame.pEncodeBuffer =
			m_EncodeBufferQueue.GetAvailable();
		if (m_bPushInfo)
			printf("		--->Get %lld frames out of encode\n",
			       m_nFramesOutEncode);
	}

	nvStatus = m_pNvCodec->NvEncLockInputBuffer(
		m_stEncodeFrame.pEncodeBuffer->stInputBfr.hInputSurface,
		(void **)&m_stEncodeFrame.pInputSurface,
		&m_stEncodeFrame.lockedPitch);
	if (nvStatus != NV_ENC_SUCCESS)
		return NULL;

	if (m_stEncodeFrame.pEncodeBuffer->stInputBfr.bufferFmt !=
	    NV_ENC_BUFFER_FORMAT_NV12_PL) //NV12
		return NULL;

	return m_stEncodeFrame.pInputSurface;
}

BOOL CSample::UnLock()
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;

	nvStatus = m_pNvCodec->NvEncUnlockInputBuffer(
		m_stEncodeFrame.pEncodeBuffer->stInputBfr.hInputSurface);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	nvStatus = m_pNvCodec->NvEncEncodeFrame(m_stEncodeFrame.pEncodeBuffer,
						NULL, m_stEncodeFrame.width,
						m_stEncodeFrame.height, 0,
						NV_ENC_CODEC_H264_GUID,
						NV_ENC_PIC_STRUCT_FRAME);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	m_nFramesIntoEncode++;
	if (m_bPushInfo)
		printf("Put %lld frames into encode\n", m_nFramesIntoEncode);

	return TRUE;
}

BOOL CSample::FlushEncoder()
{
	NVENCSTATUS nvStatus = m_pNvCodec->NvEncFlushEncoderQueue(
		m_stEOSOutputBfr.hOutputEvent);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	EncodeBuffer *pEncodeBufer = m_EncodeBufferQueue.GetPending();
	while (pEncodeBufer) {
		m_nFramesFlushOutEncode++;
		m_pNvCodec->ProcessOutput(pEncodeBufer, m_pCallback);
		pEncodeBufer = m_EncodeBufferQueue.GetPending();
		if (m_bPushInfo)
			printf("Flush %lld frames out of encode\n",
			       m_nFramesFlushOutEncode);
	}

#if defined(NV_WINDOWS)
	if (m_stCreateEncodeParams.enableEncodeAsync) {
		if (WaitForSingleObject(m_stEOSOutputBfr.hOutputEvent, 500) !=
		    WAIT_OBJECT_0) {
			assert(0);
			nvStatus = NV_ENC_ERR_GENERIC;
		}
	}
#endif

	return TRUE;
}

BOOL CSample::QueryEncodeEndStatus()
{
	if (m_bFinished) {
		return TRUE;
	} else {
		_sleep(50);
		return FALSE;
	}
}

double CSample::QueryProcessTime()
{
	printf("End: encode %lld frames in %.2lf ms , avg encode fps: %.2lf\n",
	       m_nFramesToEncode, m_llTotalProcessTime,
	       m_nFramesToEncode * 1000 / m_llTotalProcessTime);
	return m_llTotalProcessTime;
}

void CSample::ReleaseBuffers()
{
	for (uint32_t i = 0; i < m_uEncodeBufferCount; i++) {
		m_pNvCodec->NvEncDestroyInputBuffer(
			m_stEncodeBuffer[i].stInputBfr.hInputSurface);
		m_stEncodeBuffer[i].stInputBfr.hInputSurface = NULL;
		m_pNvCodec->NvEncDestroyBitstreamBuffer(
			m_stEncodeBuffer[i].stOutputBfr.hBitstreamBuffer);
		m_stEncodeBuffer[i].stOutputBfr.hBitstreamBuffer = NULL;
		if (m_stCreateEncodeParams.enableEncodeAsync) {
			m_pNvCodec->NvEncUnregisterAsyncEvent(
				m_stEncodeBuffer[i].stOutputBfr.hOutputEvent);
			nvCloseFile(
				m_stEncodeBuffer[i].stOutputBfr.hOutputEvent);
			m_stEncodeBuffer[i].stOutputBfr.hOutputEvent = NULL;
		}
	}

	if (m_stEOSOutputBfr.hOutputEvent) {
		if (m_stCreateEncodeParams.enableEncodeAsync) {
			m_pNvCodec->NvEncUnregisterAsyncEvent(
				m_stEOSOutputBfr.hOutputEvent);
			nvCloseFile(m_stEOSOutputBfr.hOutputEvent);
			m_stEOSOutputBfr.hOutputEvent = NULL;
		}
	}
}

void CSample::Exit()
{
	m_bEncoderInitialized = FALSE;

	ReleaseBuffers();

	m_pNvCodec->NvEncDestroyEncoder();
	if (m_pDevice != NULL) {
		cuCtxDestroy((CUcontext)m_pDevice);
		m_pDevice = NULL;
	}

	m_nFramesIntoEncode = 0;
	m_nFramesOutEncode = 0;
	m_nFramesFlushOutEncode = 0;
	m_nFramesToEncode = 0;
	m_llStartTemp = 0;
	m_llEndTemp = 0;
	m_llTotalProcessTime = 0;
	m_bFinished = FALSE;
}

//protected
BOOL CSample::ValidateEncodeGUID(GUID inputCodecGUID)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	GUID *encodeGUIDArray;
	UINT encodeGUIDCount = 0, encodeGUIDArraySize = 0, i = 0;
	BOOL codecFound = FALSE;

	nvStatus = m_pNvCodec->NvEncGetEncodeGUIDCount(&encodeGUIDCount);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	encodeGUIDArray = new GUID[encodeGUIDCount];
	memset(encodeGUIDArray, 0, sizeof(GUID) * encodeGUIDCount);

	encodeGUIDArraySize = 0;
	nvStatus = m_pNvCodec->NvEncGetEncodeGUIDs(
		encodeGUIDArray, encodeGUIDCount, &encodeGUIDArraySize);
	if (nvStatus != NV_ENC_SUCCESS ||
	    encodeGUIDArraySize > encodeGUIDCount) {
		delete[] encodeGUIDArray;
		return FALSE;
	}

	codecFound = FALSE;
	for (i = 0; i < encodeGUIDArraySize; i++) {
		if (inputCodecGUID == encodeGUIDArray[i]) {
			codecFound = TRUE;
			break;
		}
	}

	delete[] encodeGUIDArray;

	return codecFound;
}

BOOL CSample::ValidatePresetGUID(GUID inputPresetGuid, GUID inputCodecGuid)
{
	NVENCSTATUS nvStatus = NV_ENC_SUCCESS;
	uint32_t i, presetGUIDCount, presetGUIDArraySize;
	BOOL presetFound = FALSE;
	GUID *presetGUIDArray;

	nvStatus = m_pNvCodec->NvEncGetEncodePresetCount(inputCodecGuid,
							 &presetGUIDCount);
	if (nvStatus != NV_ENC_SUCCESS)
		return FALSE;

	presetGUIDArray = new GUID[presetGUIDCount];
	memset(presetGUIDArray, 0, sizeof(GUID) * presetGUIDCount);

	presetGUIDArraySize = 0;
	nvStatus = m_pNvCodec->NvEncGetEncodePresetGUIDs(inputCodecGuid,
							 presetGUIDArray,
							 presetGUIDCount,
							 &presetGUIDArraySize);
	if (nvStatus != NV_ENC_SUCCESS ||
	    presetGUIDArraySize > presetGUIDCount) {
		delete[] presetGUIDArray;
		return FALSE;
	}

	presetFound = FALSE;
	for (i = 0; i < presetGUIDArraySize; i++) {
		if (inputPresetGuid == presetGUIDArray[i]) {
			presetFound = TRUE;
			break;
		}
	}

	delete[] presetGUIDArray;

	return presetFound;
}
