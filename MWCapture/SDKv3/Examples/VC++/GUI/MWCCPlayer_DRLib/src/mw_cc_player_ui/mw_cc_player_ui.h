/************************************************************************************************/
// mw_cc_player_ui.h : interface of the CMWCCPlayerUI class

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

#ifndef MW_CC_PLAYER_UI_H
#define MW_CC_PLAYER_UI_H

#include "mw_cc_player_ui_manager.h"
#include "../mw_capture/mw_capture.h"
#include "../mw_opengl_render/mw_opengl_render.h"
#include "../mw_encoder/mw_encoder.h"
#include "LibMWMp4/mw_mp4.h"

#include "LibMWClosedCaption/mw_cc708_decoder.h"
#include "LibMWClosedCaption/mw_cc708_render.h"

#include "windows.h"
#include "commdlg.h"
#include "time.h"

#ifndef DFT_WINDOW_WIDTH
#define DFT_WINDOW_WIDTH 1280
#endif

#ifndef DFT_WINDOW_HEIGHT
#define DFT_WINDOW_HEIGHT 720
#endif

enum mw_capture_status_e { MW_CAPTUREING, MW_NOT_CAPTUREING, MW_CAPTURE_ERROR };

enum mw_encode_status_e {
	MW_ENCODEING,
	MW_NOT_ENCODING,
	MW_ENCODE_ERROR,
	MW_ENCODE_ERROR_NVIDIA,
	MW_ENCODE_STOPERROR
};

typedef struct mw_disk_size_s {
	uint32_t m_u32_gb;
	uint32_t m_u32_mb;
	uint32_t m_u32_kb;
	uint32_t m_u32_b;
} mw_disk_size_t;

class CMWCCPlayerUI {
public:
	static CMWCCPlayerUI *create_instance();
	static CMWCCPlayerUI *get_ui();

	void release_ui();

	bool setup_window(char *t_cs_name);

	void show();

	void cleanup_window();

protected:
	void render_ui();

public:
	static void refresh_cb(GLFWwindow *t_p_win);

	static void callback_menu_item_selected(char *t_cs_menu,
						char *t_cs_menu_item,
						int t_n_index, void *t_p_param)
	{
		if (NULL == t_p_param)
			return;

		CMWCCPlayerUI *t_p_ui = (CMWCCPlayerUI *)t_p_param;
		t_p_ui->callback_menu_item_selected_proc(
			t_cs_menu, t_cs_menu_item, t_n_index);
	}

	// video capture
	static void callback_video_capture(BYTE *pbFrame, int cbFrame,
					   UINT64 u64TimeStamp, void *pParam)
	{
		if (NULL == pParam)
			return;

		CMWCCPlayerUI *t_p_ui = (CMWCCPlayerUI *)pParam;
		t_p_ui->callback_video_capture_proc(pbFrame, cbFrame,
						    u64TimeStamp);
	}

	static void callback_anc_capture(MWCAP_SDI_ANC_PACKET t_anc_packet,
					 void *pParam, UINT64 t_u64_timestamp)
	{
		if (NULL == pParam)
			return;

		CMWCCPlayerUI *t_p_ui = (CMWCCPlayerUI *)pParam;
		t_p_ui->callback_anc_capture_proc(t_anc_packet,
						  t_u64_timestamp);
	}

	static void callback_anc_decode(int service, void *obj)
	{
		if (NULL == obj)
			return;

		CMWCCPlayerUI *t_p_ui = (CMWCCPlayerUI *)obj;
		t_p_ui->callback_anc_decode_proc(service);
	}

	static DWORD WINAPI thread_cmd_process(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if (NULL != lpThreadParameter) {
			CMWCCPlayerUI *t_p_ui =
				(CMWCCPlayerUI *)lpThreadParameter;
			return t_p_ui->thread_cmd_process_proc();
		}

		return t_dw_ret;
	}

	static DWORD WINAPI thread_encode(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if (NULL != lpThreadParameter) {
			CMWCCPlayerUI *t_p_ui =
				(CMWCCPlayerUI *)lpThreadParameter;
			return t_p_ui->thread_encode_proc();
		}

		return t_dw_ret;
	}

	static void callback_encode(void *user_ptr, const uint8_t *p_frame,
				    uint32_t frame_len,
				    mw_venc_frame_info_t *p_frame_info)
	{
		if (NULL == user_ptr)
			return;

		CMWCCPlayerUI *t_p_ui = (CMWCCPlayerUI *)user_ptr;
		t_p_ui->callback_encode_proc(p_frame, frame_len, p_frame_info);
	}

protected:
	void callback_menu_item_selected_proc(char *t_cs_menu,
					      char *t_cs_menu_item,
					      int t_n_index);

	void callback_video_capture_proc(BYTE *pbFrame, int cbFrame,
					 UINT64 u64TimeStamp);

	void callback_anc_decode_proc(int service);

	void callback_anc_capture_proc(MWCAP_SDI_ANC_PACKET t_anc_packet,
				       UINT64 t_u64_timestamp);

	DWORD thread_cmd_process_proc();

	DWORD thread_encode_proc();

	void callback_encode_proc(const uint8_t *t_p_frame,
				  uint32_t t_u32_frame_len,
				  mw_venc_frame_info_t *t_p_frame_info);

protected:
	mw_render_fourcc_t get_render_fmt_from_fourcc(DWORD t_dw_fourcc);

	mw_venc_fourcc_t get_encode_fmt_from_fourcc(DWORD t_dw_fourcc);

	void enum_encoder();

	void set_capture_status_text(char *t_cs_text,
				     mw_capture_status_e t_status);
	void set_encode_status_text(char *t_cs_text,
				    mw_encode_status_e t_status);

	bool get_disk_size(mw_disk_size_t &t_size);

private:
	CMWCCPlayerUI();
	~CMWCCPlayerUI();

private:
	static CMWCCPlayerUI *s_ui;
	static uint32_t s_u32_ref;

	static GLFWwindow *s_p_window;
	static HWND s_h_window;

	ImGuiContext *m_p_imgui_ctx;

	CMWCCPlayerUIManager *m_p_ui_manager;
	CMCCPlayerMenuBar *m_p_menubar;
	CMWCCSetting *m_p_cc_setting;
	CMWEncoderAdapter *m_p_encoder_adapter;
	CMWCCOverlayText *m_p_overlay_capture;
	CMWCCLayer *m_p_cc_layer;

	CMWCapture *m_p_capture;
	cmw_video_capture_param_s m_video_capture_param;
	int m_n_capture_index;
	int m_n_ui_select;

	mw_cc708_decoder_t *m_p_cc708_decoder;
	mw_cc_render_t *m_p_cc_render;
	mw_cc_font_t m_mcf_font;

	bool m_b_show_cc708;
	bool m_b_show_cc608;
	int m_n_cc608_out;

	bool m_b_set_anc_track;

	CMWOpenGLRender *m_p_opengl_render;
	int m_n_data_size;

	CMWRenderBuffer *m_p_render_buffer;
	int m_n_buffer_size;

	char m_cs_menu_file[16];
	char m_cs_menu_start_record[32];
	char m_cs_menu_stop_record[32];
	char m_cs_encoder_adapter[32];

	OPENFILENAME m_ofn;
	HANDLE m_h_thread_cmd_proc;
	HANDLE m_h_event_openfile;
	HANDLE m_h_event_stop_record;
	HANDLE m_h_event_device_changed;
	HANDLE m_h_event_exit_thread;

	HANDLE m_h_thread_encode;
	HANDLE m_h_event_encode;
	HANDLE m_h_event_exit_thread_encode;

	char m_cs_disk_size[256];

	mw_render_ctrl_t m_render_ctrl;

	char m_cs_menu_device[16];

	char m_cs_menu_cc_settings[16];

	float m_f_capture_fps;
	int m_n_capture_num;
	char m_cs_capture_status[512];
	clock_t m_last_capture_clock;
	uint64_t m_u64_total_capture_num;
	mw_capture_status_e m_capture_status;

	float m_f_encode_fps;
	int m_n_encode_num;
	char m_cs_encode_status[512];
	clock_t m_last_encode_clock;
	mw_disk_size_t m_file_size;
	uint64_t m_u64_total_encode_size;
	uint64_t m_u64_total_encode_num;
	uint64_t m_u64_current_file_size;
	mw_encode_status_e m_encode_status;

	char m_cs_overlay_text[1024];

	char m_cs_encoding_adapter[1024];
	wchar_t m_wcs_file_name[1024];
	wchar_t m_wcs_file_name_main[1024];
	wchar_t m_wcs_file_name_suffixes[16];
	int m_n_file_name_index;
	wchar_t m_wcs_file_name_sub[1024];
	wchar_t m_wcs_dir[256];
	clock_t m_clock_start;

	mw_mp4_handle_t m_p_file;
	CRITICAL_SECTION m_cs_lock_file;

	CMWEncoder *m_p_encoder;
	vector<mw_venc_gpu_info_t> m_vec_gpu;
	vector<int> m_vec_index;
	mw_venc_param_t m_venc_param;
	bool m_b_need_set_mp4;

	unsigned char m_cs_render[256];
	int m_n_len_render;

	char *m_p_encode_buffer;
	uint64_t m_u64_timestamp;
	CRITICAL_SECTION m_cs_lock;

	CRITICAL_SECTION m_cs_lock_cc;
	unsigned char *m_p_cc_screen;
	int m_n_cc_width;
	int m_n_cc_height;
	int m_n_cc_size;
	int m_n_target_x;
	int m_n_target_y;
	int m_n_target_width;
	int m_n_target_height;

	unsigned char *m_p_cc_change_screen;
	int m_n_change_frame;
	int m_n_change_width;
	int m_n_change_height;
	int m_n_change_size;

	GLuint m_glsl_cc;

	mw_cc_setting_param_t m_cc_param;
};

#endif