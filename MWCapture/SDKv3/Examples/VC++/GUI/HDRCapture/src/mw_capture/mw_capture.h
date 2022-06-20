/************************************************************************************************/
// mw_capture.h : interface of the CMWCapture class

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

#ifndef MW_CAPTURE_H
#define MW_CAPTURE_H

#include "vector"
#include "LibMWCapture/MWCapture.h"
using namespace std;

#include "..\mw_opengl_render\mw_render_buffer.h"

struct cmw_channel_info_s {
	MWCAP_CHANNEL_INFO m_channel_info;
	WCHAR m_wcs_path[256];
};

struct cmw_video_capture_param_s {
	int m_n_width;
	int m_n_height;
	DWORD m_dw_fourcc;
	unsigned long m_ul_frameduration;
};

class CMWCapture {
public:
	CMWCapture();
	~CMWCapture();

public:
	void init_capture();
	void deinit_capture();

	bool refresh_device();

	int get_channel_count();

	bool get_channel_info(int t_n_index,
			      cmw_channel_info_s *t_channel_info);

	MW_FAMILY_ID get_family_id();

	bool get_is_captureing();

	bool open_channel(int t_n_index);

	bool start_video_capture(cmw_video_capture_param_s t_param,
				 VIDEO_CAPTURE_CALLBACK t_video_callback,
				 void *t_p_param,
				 CMWRenderBuffer *t_p_render_buffer);

	void stop_video_capture();

	void close_channel();

	bool is_hdr();

	bool get_hdr_infoframe_packet(HDMI_INFOFRAME_PACKET *t_p_packet);

public:
	static DWORD WINAPI VideoThreadProc(LPVOID pvParam)
	{
		CMWCapture *pThis = (CMWCapture *)pvParam;
		MWCAP_CHANNEL_INFO mci;

		MWGetChannelInfo(pThis->m_h_channel, &mci);

		if (0 == strcmp(mci.szFamilyName, "Eco Capture")) {
			return pThis->VideoEcoThreadProc();

		} else {
			return pThis->VideoProThreadProc();
		}
	}

	DWORD VideoProThreadProc();
	DWORD VideoEcoThreadProc();

protected:
	void trans_fourcc(char *t_cs_fourcc, uint32_t t_u32_fourcc);

protected:
	vector<cmw_channel_info_s> m_vec_channel_info;

	int m_n_index;
	HCHANNEL m_h_channel;
	HANDLE m_h_video;

	HANDLE m_h_pcie_video;
	bool m_b_pcie_capturing;
	HANDLE m_h_event_exit_video_thread;

	cmw_video_capture_param_s m_video_capture_param;

	VIDEO_CAPTURE_CALLBACK m_video_callback;
	void *m_p_caller;

	bool m_b_captureing;
	bool m_b_hdr;

	HDMI_INFOFRAME_PACKET m_hdr_packet;

	CMWRenderBuffer *m_p_render_buffer;
};

#endif
