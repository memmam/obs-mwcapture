/************************************************************************************************/
// mw_capture.cpp : implementation of the CMWCapture class

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

#include "mw_capture.h"

CMWCapture::CMWCapture()
{
	m_vec_channel_info.clear();

	m_n_index = -1;
	m_h_channel = NULL;
	m_h_video = NULL;

	m_b_captureing = false;

	m_h_pcie_video = NULL;
	m_b_pcie_capturing = false;
	m_h_event_exit_video_thread = NULL;

	m_h_anc = NULL;
	m_h_event_exit_anc_thread = NULL;

	m_video_callback = NULL;
	m_p_caller = NULL;
}

CMWCapture::~CMWCapture() {}

void CMWCapture::init_capture()
{
	MWCaptureInitInstance();
}

void CMWCapture::deinit_capture()
{
	MWCaptureExitInstance();
}

bool CMWCapture::refresh_device()
{
	bool t_b_ret = false;
	MW_RESULT t_ret = MWRefreshDevice();
	if (t_ret != MW_SUCCEEDED)
		return t_b_ret;

	m_vec_channel_info.clear();
	int t_n_channel_count = MWGetChannelCount();
	for (int i = 0; i < t_n_channel_count; i++) {
		cmw_channel_info_s t_info_s;
		MWCAP_CHANNEL_INFO t_info;
		t_ret = MWGetChannelInfoByIndex(i, &t_info);
		if (t_ret == MW_SUCCEEDED)
			t_info_s.m_channel_info = t_info;

		t_ret = MWGetDevicePath(i, t_info_s.m_wcs_path);

		// only eco and pro
		if (t_info.wFamilyID == MW_FAMILY_ID_USB_CAPTURE)
			continue;

		// only sdi
		HCHANNEL t_h_channel = MWOpenChannelByPath(t_info_s.m_wcs_path);
		if (t_h_channel == NULL)
			continue;

		DWORD t_dw_count = 0;
		t_ret = MWGetVideoInputSourceArray(t_h_channel, NULL,
						   &t_dw_count);
		if (t_ret != MW_SUCCEEDED) {
			MWCloseChannel(t_h_channel);
			continue;
		}
		//number of input source types is less than 6
		DWORD t_arr_source[16] = {0};
		t_ret = MWGetVideoInputSourceArray(t_h_channel, t_arr_source,
						   &t_dw_count);
		if (t_ret != MW_SUCCEEDED) {
			MWCloseChannel(t_h_channel);
			continue;
		}
		MWCloseChannel(t_h_channel);
		for (int j = 0; j < t_dw_count; j++) {
			if (INPUT_TYPE(t_arr_source[j]) ==
			    MWCAP_VIDEO_INPUT_TYPE_SDI) {
				m_vec_channel_info.push_back(t_info_s);
			}
		}
	}

	t_b_ret = true;
	return t_b_ret;
}

int CMWCapture::get_channel_count()
{
	return m_vec_channel_info.size();
}

bool CMWCapture::get_channel_info(int t_n_index,
				  cmw_channel_info_s *t_channel_info)
{
	bool t_b_ret = false;

	if (t_n_index < 0 || t_n_index >= m_vec_channel_info.size())
		return t_b_ret;

	*t_channel_info = m_vec_channel_info.at(t_n_index);
	t_b_ret = true;

	return t_b_ret;
}

MW_FAMILY_ID CMWCapture::get_family_id()
{
	MW_FAMILY_ID t_family_id = MW_FAMILY_ID_PRO_CAPTURE;

	t_family_id = (MW_FAMILY_ID)m_vec_channel_info.at(m_n_index)
			      .m_channel_info.wFamilyID;

	return t_family_id;
}

bool CMWCapture::get_is_captureing()
{
	return m_b_captureing;
}

bool CMWCapture::open_channel(int t_n_index)
{
	bool t_b_ret = false;

	if (t_n_index < 0 || t_n_index >= m_vec_channel_info.size())
		return t_b_ret;

	HCHANNEL t_h_channel = MWOpenChannelByPath(
		m_vec_channel_info.at(t_n_index).m_wcs_path);
	if (t_h_channel == NULL)
		return t_b_ret;

	m_h_channel = t_h_channel;
	m_n_index = t_n_index;
	t_b_ret = true;

	return t_b_ret;
}

bool CMWCapture::start_video_capture(cmw_video_capture_param_s t_param,
				     VIDEO_CAPTURE_CALLBACK t_video_callback,
				     void *t_p_param,
				     CMWRenderBuffer *t_p_render_buffer)
{
	bool t_b_ret = false;

	m_p_render_buffer = NULL;
	m_p_render_buffer = t_p_render_buffer;

	if (t_param.m_n_width <= 0 || t_param.m_n_height <= 0 ||
	    t_param.m_ul_frameduration <= 0) {
		m_b_captureing = false;
		return t_b_ret;
	}

	if (m_h_channel == NULL) {
		m_b_captureing = false;
		return t_b_ret;
	}

	m_video_capture_param = t_param;
	m_video_callback = t_video_callback;
	m_p_caller = t_p_param;

	MWCAP_VIDEO_CAPS t_caps;
	MWGetVideoCaps(m_h_channel, &t_caps);

	HANDLE t_h_video = NULL;
	if (m_vec_channel_info.at(m_n_index).m_channel_info.wFamilyID ==
	    MW_FAMILY_ID_USB_CAPTURE) {
		MWCloseChannel(m_h_channel);
		m_h_channel = NULL;
		m_b_captureing = false;
		return false;
	} else {
		m_h_event_exit_video_thread =
			CreateEvent(NULL, FALSE, FALSE, NULL);
		m_h_pcie_video =
			CreateThread(NULL, 0, VideoThreadProc, this, 0, NULL);
		if (m_h_pcie_video == NULL) {
			MWCloseChannel(m_h_channel);
			m_h_channel = NULL;
			m_b_captureing = false;
			return t_b_ret;
		} else {
			m_b_captureing = true;
			t_b_ret = true;
		}
	}

	m_h_video = t_h_video;

	return t_b_ret;
}

void CMWCapture::stop_video_capture()
{
	if (m_h_video != NULL) {
		MWDestoryVideoCapture(m_h_video);
		m_h_video = NULL;
	}

	if (m_h_pcie_video != NULL) {
		m_b_pcie_capturing = false;
		SetEvent(m_h_event_exit_video_thread);
		int t_n_ret = WaitForSingleObject(m_h_pcie_video, INFINITE);
		if (m_h_event_exit_video_thread != NULL) {
			CloseHandle(m_h_event_exit_video_thread);
			m_h_event_exit_video_thread = NULL;
		}
		if (m_h_pcie_video != NULL) {
			CloseHandle(m_h_pcie_video);
			m_h_pcie_video = 0;
		}
	}

	m_video_callback = NULL;

	m_b_captureing = false;
}

bool CMWCapture::start_anc_capture(ANC_CAPTURE_CALLBACK t_anc_callback,
				   void *t_p_param)
{
	if (NULL == m_h_channel)
		return false;

	m_anc_callback = t_anc_callback;
	m_p_anc_caller = t_p_param;

	m_h_event_exit_anc_thread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_b_anc_capturing = true;
	m_h_anc = CreateThread(NULL, 0, ANCThread, this, 0, NULL);
	if (NULL == m_h_anc) {
		m_b_anc_capturing = false;
		CloseHandle(m_h_event_exit_anc_thread);
		m_h_event_exit_anc_thread = NULL;
		return false;
	}

	return true;
}

void CMWCapture::stop_anc_capture()
{
	if (m_h_anc) {
		m_b_anc_capturing = false;
		SetEvent(m_h_event_exit_anc_thread);
		int t_n_ret = WaitForSingleObject(m_h_anc, INFINITE);
		if (m_h_event_exit_anc_thread != NULL) {
			CloseHandle(m_h_event_exit_anc_thread);
			m_h_event_exit_anc_thread = NULL;
		}
		if (m_h_anc != NULL) {
			CloseHandle(m_h_anc);
			m_h_anc = 0;
		}
	}

	m_anc_callback = NULL;
	m_p_anc_caller = NULL;
}

void CMWCapture::close_channel()
{
	if (m_h_channel != NULL) {
		MWCloseChannel(m_h_channel);
		m_h_channel = NULL;
	}
}

DWORD CMWCapture::VideoProThreadProc()
{
	// Preview
	DWORD cbStride = FOURCC_CalcMinStride(m_video_capture_param.m_dw_fourcc,
					      m_video_capture_param.m_n_width,
					      2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(
		m_video_capture_param.m_dw_fourcc,
		m_video_capture_param.m_n_width,
		m_video_capture_param.m_n_height, cbStride);

	BYTE *byBuffer = NULL;
	byBuffer = new BYTE[dwFrameSize];
	memset(byBuffer, 0xFF, dwFrameSize);

	// Wait Events
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	HANDLE hTimerEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_b_pcie_capturing = true;

	MW_RESULT xr;
	do {
		xr = MWStartVideoCapture(m_h_channel, hCaptureEvent);
		if (xr != MW_SUCCEEDED)
			break;

		HTIMER hTimerNotify = MWRegisterTimer(m_h_channel, hTimerEvent);
		if (hTimerNotify == NULL)
			break;

		DWORD dwFrameCount = 0;

		LONGLONG llBegin = 0LL;
		xr = MWGetDeviceTime(m_h_channel, &llBegin);
		if (xr != MW_SUCCEEDED)
			break;

		LONGLONG llExpireTime = llBegin;
		DWORD dwFrameDuration =
			m_video_capture_param.m_ul_frameduration;

		MWCAP_VIDEO_CAPTURE_STATUS captureStatus;

		LONGLONG llLast = llBegin;
		LONGLONG llNow = 0;

		BOOLEAN t_b_bottom_up = FALSE;
		if (m_video_capture_param.m_dw_fourcc == MWFOURCC_BGRA ||
		    m_video_capture_param.m_dw_fourcc == MWFOURCC_RGBA ||
		    m_video_capture_param.m_dw_fourcc == MWFOURCC_ARGB ||
		    m_video_capture_param.m_dw_fourcc == MWFOURCC_ABGR)
			t_b_bottom_up = FALSE;

		vector<BYTE *> t_vec_buffer;

		MWPinVideoBuffer(m_h_channel, byBuffer, dwFrameSize);
		t_vec_buffer.push_back(byBuffer);

		if (m_p_render_buffer != NULL) {
			int t_n_num = m_p_render_buffer->get_buffer_num();
			for (int i = 0; i < t_n_num; i++) {
				cmw_buffer_s *t_p_buffer =
					m_p_render_buffer->get_buffer_by_index(
						i);
				if (t_p_buffer != NULL) {
					if (t_p_buffer->m_p_buffer != NULL) {
						MWPinVideoBuffer(
							m_h_channel,
							t_p_buffer->m_p_buffer,
							dwFrameSize);
						t_vec_buffer.push_back(
							t_p_buffer->m_p_buffer);
					}
				}
			}
		}

		while (m_b_pcie_capturing) {

			xr = MWGetDeviceTime(m_h_channel, &llNow);
			if (xr != MW_SUCCEEDED)
				continue;

			//llExpireTime = llNow + dwFrameDuration;
			llExpireTime = llExpireTime + dwFrameDuration;

			llExpireTime = llExpireTime < llNow
					       ? llNow + dwFrameDuration
					       : llExpireTime;

			xr = MWScheduleTimer(m_h_channel, hTimerNotify,
					     llExpireTime);
			if (xr != MW_SUCCEEDED) {
				continue;
			}

			HANDLE aEventNotify[2] = {m_h_event_exit_video_thread,
						  hTimerEvent};
			DWORD dwRet = WaitForMultipleObjects(2, aEventNotify,
							     FALSE, INFINITE);
			if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED) {
				printf("get m_h_event_exit_video_thread\n");
				break;
			} else if (dwRet == WAIT_OBJECT_0 + 1) {
				MWCAP_VIDEO_BUFFER_INFO videoBufferInfo;
				if (MW_SUCCEEDED !=
				    MWGetVideoBufferInfo(m_h_channel,
							 &videoBufferInfo))
					continue;

				MWCAP_VIDEO_FRAME_INFO videoFrameInfo;
				xr = MWGetVideoFrameInfo(
					m_h_channel,
					videoBufferInfo.iNewestBufferedFullFrame,
					&videoFrameInfo);
				if (xr != MW_SUCCEEDED)
					continue;

				BYTE *t_p_data = byBuffer;
				cmw_buffer_s *t_p_mwbuffer = NULL;
				if (m_p_render_buffer != NULL) {
					t_p_mwbuffer =
						m_p_render_buffer
							->get_buffer_to_fill();
					if (t_p_mwbuffer != NULL) {
						if (t_p_mwbuffer->m_p_buffer !=
						    NULL)
							t_p_data =
								t_p_mwbuffer
									->m_p_buffer;
					}
				}

				xr = MWCaptureVideoFrameToVirtualAddressEx(
					m_h_channel,
					MWCAP_VIDEO_FRAME_ID_NEWEST_BUFFERED,
					t_p_data, dwFrameSize, cbStride,
					t_b_bottom_up, NULL,
					m_video_capture_param.m_dw_fourcc,
					m_video_capture_param.m_n_width,
					m_video_capture_param.m_n_height, 0, 0,
					NULL, NULL, 0, 100, 0, 100, 0,
					MWCAP_VIDEO_DEINTERLACE_BLEND,
					MWCAP_VIDEO_ASPECT_RATIO_IGNORE, NULL,
					NULL, 0, 0,
					MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
					MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
					MWCAP_VIDEO_SATURATION_UNKNOWN);

				WaitForSingleObject(hCaptureEvent, INFINITE);

				xr = MWGetVideoCaptureStatus(m_h_channel,
							     &captureStatus);

				if (t_p_mwbuffer != NULL)
					m_p_render_buffer->put_buffer_filled(
						t_p_mwbuffer);

				if (m_video_callback != NULL) {
					xr = MWGetDeviceTime(m_h_channel,
							     &llNow);
					m_video_callback(t_p_data, dwFrameSize,
							 llNow, m_p_caller);
				}
			}
		}

		while (!t_vec_buffer.empty()) {
			MWUnpinVideoBuffer(m_h_channel, t_vec_buffer.back());
			t_vec_buffer.pop_back();
		}

		xr = MWUnregisterTimer(m_h_channel, hTimerNotify);
		xr = MWStopVideoCapture(m_h_channel);

	} while (FALSE);

	CloseHandle(hCaptureEvent);
	CloseHandle(hTimerEvent);

	if (byBuffer != NULL) {
		delete[] byBuffer;
	}

	return 0;
}

DWORD CMWCapture::VideoEcoThreadProc()
{
	//create event and notify
	HANDLE hCaptureEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	MW_RESULT xr = MW_FAILED;
	LONGLONG llNow = 0;

	//start video eco capture
	LONGLONG t_ll_frame_dyration = -1;
	if (m_video_capture_param.m_ul_frameduration > 0)
		t_ll_frame_dyration = m_video_capture_param.m_ul_frameduration;

	MWCAP_VIDEO_ECO_CAPTURE_OPEN ecoCaptureOpen = {
		hCaptureEvent, m_video_capture_param.m_dw_fourcc,
		m_video_capture_param.m_n_width,
		m_video_capture_param.m_n_height, t_ll_frame_dyration};
	xr = MWStartVideoEcoCapture(m_h_channel, &ecoCaptureOpen);

	//set video capture frame
	MWCAP_VIDEO_ECO_CAPTURE_FRAME videoFrame[3] = {0};
	int iCompleted = 0;
	DWORD cbStride = FOURCC_CalcMinStride(m_video_capture_param.m_dw_fourcc,
					      m_video_capture_param.m_n_width,
					      2);
	DWORD dwFrameSize = FOURCC_CalcImageSize(
		m_video_capture_param.m_dw_fourcc,
		m_video_capture_param.m_n_width,
		m_video_capture_param.m_n_height, cbStride);
	for (int i = 0; i < sizeof(videoFrame) / sizeof(videoFrame[0]); i++) {
		videoFrame[i].pvFrame = (MWCAP_PTR64)malloc(dwFrameSize);
		videoFrame[i].cbFrame = dwFrameSize;
		videoFrame[i].cbStride = cbStride;
		videoFrame[i].pvContext = &(videoFrame[i]);
		xr = MWCaptureSetVideoEcoFrame(m_h_channel, &(videoFrame[i]));
	}

	LONGLONG llBegin = 0LL;
	xr = MWGetDeviceTime(m_h_channel, &llBegin);
	LONGLONG llLast = llBegin;
	DWORD dwFrameCount = 0;

	m_b_pcie_capturing = true;

	//video capture loop
	while (m_b_pcie_capturing) {
		HANDLE aEventNotify[] = {m_h_event_exit_video_thread,
					 hCaptureEvent};
		DWORD dwRet = WaitForMultipleObjects(
			sizeof(aEventNotify) / sizeof(aEventNotify[0]),
			aEventNotify, FALSE, INFINITE);
		if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_FAILED) {
			break;
		} else if (dwRet == WAIT_OBJECT_0 + 1) {
			MWCAP_VIDEO_ECO_CAPTURE_STATUS captureStatus;
			while (m_b_pcie_capturing) {
				xr = MWGetVideoEcoCaptureStatus(m_h_channel,
								&captureStatus);
				if (xr == MW_FAILED ||
				    captureStatus.pvFrame == NULL) {
					break;
				}

				BYTE *t_p_data = NULL;
				cmw_buffer_s *t_p_mwbuffer = NULL;
				if (m_p_render_buffer != NULL) {
					t_p_mwbuffer =
						m_p_render_buffer
							->get_buffer_to_fill();
					if (t_p_mwbuffer != NULL) {
						if (t_p_mwbuffer->m_p_buffer !=
						    NULL)
							t_p_data =
								t_p_mwbuffer
									->m_p_buffer;
					}
				}

				if (t_p_data != NULL)
					memcpy(t_p_data, captureStatus.pvFrame,
					       dwFrameSize);

				if (t_p_mwbuffer != NULL)
					m_p_render_buffer->put_buffer_filled(
						t_p_mwbuffer);

				if (m_video_callback != NULL) {
					xr = MWGetDeviceTime(m_h_channel,
							     &llNow);
					m_video_callback(
						(BYTE *)captureStatus.pvFrame,
						dwFrameSize, llNow, m_p_caller);
				}

				xr = MWCaptureSetVideoEcoFrame(
					m_h_channel,
					(MWCAP_VIDEO_ECO_CAPTURE_FRAME *)
						videoFrame[iCompleted]
							.pvContext);
				iCompleted = (iCompleted + 1) %
					     (sizeof(videoFrame) /
					      sizeof(videoFrame[0]));
			}
		}
	}

	//cleanup
	if (m_h_channel)
		MWStopVideoEcoCapture(m_h_channel);

	if (hCaptureEvent) {
		CloseHandle(hCaptureEvent);
		hCaptureEvent = NULL;
	}

	for (int i = 0; i < sizeof(videoFrame) / sizeof(videoFrame[0]); i++) {
		if (videoFrame[i].pvFrame) {
			free(videoFrame[i].pvFrame);
			videoFrame[i].pvFrame = NULL;
		}
	}

	return 0;
}

DWORD CMWCapture::ANCThreadProc()
{
	//clear pre settings
	if (m_h_channel != NULL) {
		for (int i = 0; i < 4; i++) {
			MWCaptureSetSDIANCType(m_h_channel, i, FALSE, FALSE,
					       0x0, 0x0);
		}

		MW_RESULT ret = MWCaptureSetSDIANCType(
			m_h_channel, 0 /*byIndex*/, FALSE /*bHANC*/,
			TRUE /*bVANC*/, 0x61 /*byDID*/, 0x01 /*bySDID*/);

		// Clear old packets
		MWCAP_SDI_ANC_PACKET ancPacket = {0};
		do {
			ret = MWCaptureGetSDIANCPacket(m_h_channel, &ancPacket);
		} while (ret == MW_SUCCEEDED && ancPacket.byDID != 0);

		HANDLE hNotifyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		HNOTIFY hNotify =
			MWRegisterNotify(m_h_channel, hNotifyEvent,
					 MWCAP_NOTIFY_NEW_SDI_ANC_PACKET);

		HANDLE aEventNotify[] = {m_h_event_exit_anc_thread,
					 hNotifyEvent};
		while (m_b_anc_capturing) {
			DWORD t_dw_ret = WaitForMultipleObjects(
				2, aEventNotify, FALSE, INFINITE);
			if (WAIT_OBJECT_0 == t_dw_ret)
				break;
			else if (WAIT_OBJECT_0 + 1 == t_dw_ret) {
				ULONGLONG ullStatusBits;
				ret = MWGetNotifyStatus(m_h_channel, hNotify,
							&ullStatusBits);

				if (ullStatusBits &
				    MWCAP_NOTIFY_NEW_SDI_ANC_PACKET) {
					do {
						ret = MWCaptureGetSDIANCPacket(
							m_h_channel,
							&ancPacket);
						if (ret == MW_SUCCEEDED &&
						    ancPacket.byDID != 0) {
							LONGLONG llNow = 0;
							ret = MWGetDeviceTime(
								m_h_channel,
								&llNow);
							m_anc_callback(
								ancPacket,
								m_p_anc_caller,
								llNow);
						}
					} while (ret == MW_SUCCEEDED &&
						 ancPacket.byDID != 0);
				}
			}
		}
		MWUnregisterNotify(m_h_channel, hNotify);
		CloseHandle(hNotifyEvent);
	}
	return 0;
}

void CMWCapture::trans_fourcc(char *t_cs_fourcc, uint32_t t_u32_fourcc)
{
	switch (t_u32_fourcc) {
	case MWFOURCC_GREY:
		sprintf(t_cs_fourcc, "GREY");
		break;
	case MWFOURCC_Y800:
		sprintf(t_cs_fourcc, "Y800");
		break;
	case MWFOURCC_Y8:
		sprintf(t_cs_fourcc, "Y8");
		break;
	case MWFOURCC_Y16:
		sprintf(t_cs_fourcc, "Y16");
		break;
	case MWFOURCC_RGB15:
		sprintf(t_cs_fourcc, "RGB15");
		break;
	case MWFOURCC_RGB16:
		sprintf(t_cs_fourcc, "RGB16");
		break;
	case MWFOURCC_RGB24:
		sprintf(t_cs_fourcc, "RGB16");
		break;
	case MWFOURCC_BGRA:
		sprintf(t_cs_fourcc, "BGRA");
		break;
	case MWFOURCC_ABGR:
		sprintf(t_cs_fourcc, "ABGR");
		break;
	case MWFOURCC_NV16:
		sprintf(t_cs_fourcc, "NV16");
		break;
	case MWFOURCC_NV61:
		sprintf(t_cs_fourcc, "NV61");
		break;
	case MWFOURCC_I422:
		sprintf(t_cs_fourcc, "I422");
		break;
	case MWFOURCC_YV16:
		sprintf(t_cs_fourcc, "YV16");
		break;
	case MWFOURCC_YUY2:
		sprintf(t_cs_fourcc, "YUY2");
		break;
	case MWFOURCC_YUYV:
		sprintf(t_cs_fourcc, "YUYV");
		break;
	case MWFOURCC_UYVY:
		sprintf(t_cs_fourcc, "UYVY");
		break;
	case MWFOURCC_YVYU:
		sprintf(t_cs_fourcc, "YVYU");
		break;
	case MWFOURCC_VYUY:
		sprintf(t_cs_fourcc, "VYUY");
		break;
	case MWFOURCC_I420:
		sprintf(t_cs_fourcc, "I420");
		break;
	case MWFOURCC_IYUV:
		sprintf(t_cs_fourcc, "IYUV");
		break;
	case MWFOURCC_NV12:
		sprintf(t_cs_fourcc, "NV12");
		break;
	case MWFOURCC_YV12:
		sprintf(t_cs_fourcc, "YV12");
		break;
	case MWFOURCC_NV21:
		sprintf(t_cs_fourcc, "NV21");
		break;
	case MWFOURCC_P010:
		sprintf(t_cs_fourcc, "P010");
		break;
	case MWFOURCC_P210:
		sprintf(t_cs_fourcc, "P210");
		break;
	case MWFOURCC_IYU2:
		sprintf(t_cs_fourcc, "IYU2");
		break;
	case MWFOURCC_V308:
		sprintf(t_cs_fourcc, "V308");
		break;
	case MWFOURCC_AYUV:
		sprintf(t_cs_fourcc, "AYUV");
		break;
	case MWFOURCC_UYVA:
		sprintf(t_cs_fourcc, "UYVA");
		break;
	case MWFOURCC_V408:
		sprintf(t_cs_fourcc, "V408");
		break;
	case MWFOURCC_VYUA:
		sprintf(t_cs_fourcc, "VYUA");
		break;
	//case MWFOURCC_V210:
	//	sprintf(t_cs_fourcc,"V210");
	//	break;
	case MWFOURCC_Y410:
		sprintf(t_cs_fourcc, "Y410");
		break;
	case MWFOURCC_V410:
		sprintf(t_cs_fourcc, "V410");
		break;
	case MWFOURCC_RGB10:
		sprintf(t_cs_fourcc, "RGB10");
		break;
	case MWFOURCC_BGR10:
		sprintf(t_cs_fourcc, "BGR10");
		break;
	default:
		break;
	}
}