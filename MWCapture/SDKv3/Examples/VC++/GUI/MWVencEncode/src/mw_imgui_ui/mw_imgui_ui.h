/************************************************************************************************/
// mw_imgui_ui.h : interface of the CMWIMGUIUI class

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

#ifndef MW_IMGUI_UI_H
#define MW_IMGUI_UI_H

#include "mw_imgui_ui_manager.h"
#include "../mw_capture/mw_capture.h"
#include "../mw_opengl_render/mw_render_buffer.h"
#include "../mw_opengl_render/mw_opengl_render.h"
#include "../mw_encoder/mw_encoder.h"
#include "time.h"
#include "commdlg.h"
#include <direct.h>

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

class CMWIMGUIUI {
public:
	static CMWIMGUIUI *get_ui(); // get ui

	static CMWIMGUIUI *create_instance();

	void release_ui();

	bool setup_window(char *t_cs_name);

	void show();

	void cleanup_window();

protected:
	void render_ui();

public:
	static void refresh_cb(GLFWwindow *t_p_win);

	// device clicked
	static void callback_menu_item_selected(char *t_cs_menu,
						char *t_cs_menu_item,
						int t_n_index, void *t_p_param)
	{
		if (t_p_param == NULL)
			return;

		CMWIMGUIUI *t_p_ui = (CMWIMGUIUI *)t_p_param;
		t_p_ui->callback_menu_item_selected_proc(
			t_cs_menu, t_cs_menu_item, t_n_index);
	}

	void callback_menu_item_selected_proc(char *t_cs_menu,
					      char *t_cs_menu_item,
					      int t_n_index);

	// video capture
	static void callback_video_capture(BYTE *pbFrame, int cbFrame,
					   UINT64 u64TimeStamp, void *pParam)
	{
		if (pParam == NULL)
			return;

		CMWIMGUIUI *t_p_ui = (CMWIMGUIUI *)pParam;
		t_p_ui->callback_video_capture_proc(pbFrame, cbFrame,
						    u64TimeStamp);
	}

	void callback_video_capture_proc(BYTE *pbFrame, int cbFrame,
					 UINT64 u64TimeStamp);

	// video capture param changed
	static void callback_video_capture_param_changed(
		cmw_ui_video_capture_param_s t_param, void *t_p_param)
	{
		if (t_p_param == NULL)
			return;

		CMWIMGUIUI *t_p_ui = (CMWIMGUIUI *)t_p_param;
		t_p_ui->callback_video_capture_param_changed_proc(t_param);
	}

	void callback_video_capture_param_changed_proc(
		cmw_ui_video_capture_param_s t_param);

	// encode output callback
	static void callback_encode(void *user_ptr, const uint8_t *p_frame,
				    uint32_t frame_len,
				    mw_venc_frame_info_t *p_frame_info)
	{
		if (user_ptr == NULL)
			return;

		CMWIMGUIUI *t_p_ui = (CMWIMGUIUI *)user_ptr;
		t_p_ui->callback_encode_proc(p_frame, frame_len, p_frame_info);
	}

	void callback_encode_proc(const uint8_t *p_frame, uint32_t frame_len,
				  mw_venc_frame_info_t *p_frame_info);

	static DWORD WINAPI thread_cmd_process(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if (lpThreadParameter != NULL) {
			CMWIMGUIUI *t_p_this = (CMWIMGUIUI *)lpThreadParameter;
			return t_p_this->thread_cmd_process_proc();
		}

		return t_dw_ret;
	}

	DWORD thread_cmd_process_proc();

	static DWORD WINAPI thread_encode(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if (lpThreadParameter != NULL) {
			CMWIMGUIUI *t_p_this = (CMWIMGUIUI *)lpThreadParameter;
			return t_p_this->thread_encode_proc();
		}

		return t_dw_ret;
	}

	DWORD thread_encode_proc();

	static void callback_enable_encode(bool t_b_enable, void *t_p_param)
	{
		CMWIMGUIUI *t_p_ui = (CMWIMGUIUI *)t_p_param;
		if (t_p_ui != NULL)
			t_p_ui->callback_enable_encode_proc(t_b_enable);
	}

	void callback_enable_encode_proc(bool t_b_enable);

protected:
	DWORD get_fourcc_from_str(char *t_cs_color);
	mw_render_fourcc_t get_render_fmt_from_fourcc(DWORD t_dw_fourcc);
	mw_venc_fourcc_t get_vf_fourcc_from_fourcc(DWORD t_dw_fourcc);

	void set_capture_status_text(char *t_cs_text,
				     mw_capture_status_e t_status);
	void set_encode_status_text(char *t_cs_text,
				    mw_encode_status_e t_status);

	void get_amd_mem(mw_venc_amd_mem_type t_type);

	bool get_disk_size(mw_disk_size_t &t_size);

private:
	CMWIMGUIUI();
	~CMWIMGUIUI();

private:
	static CMWIMGUIUI *s_ui;
	static int s_n_ref;
	static GLFWwindow *s_p_window;
	static HWND s_h_hwnd;

	// dialog for file
	OPENFILENAME m_ofn;
	HANDLE m_h_thread_cmd_proc;
	HANDLE m_h_event_openfile;
	HANDLE m_h_event_stop_record;
	HANDLE m_h_event_device_changed;
	HANDLE m_h_event_exit_thread;

	HANDLE m_h_thread_encode;
	HANDLE m_h_event_encode;
	HANDLE m_h_event_exit_thread_encode;

	ImGuiContext *m_p_imgui_context;
	ImVec4 m_vec4_clear_color;

	CMWIMGUIUIManager *m_p_ui_manager;
	CMWIMGUIWidgetMenuBar *m_p_ui_menubar;
	CMWIMGUIWidgetVideoParam *m_p_ui_videoparam;
	CMWIMGUIWidgetEncodeParam *m_p_ui_encodeparam;
	CMWIMGUIWidgetOverlayText *m_p_ui_overlay_capture;

	CMWCapture *m_p_capture;
	cmw_video_capture_param_s m_video_capture_param;
	int m_n_capture_index;
	int m_n_ui_select;

	CMWOpenGLRender *m_p_opengl_render;
	int m_n_data_size;

	unsigned char *m_p_yuy2_data;
	CMWRenderBuffer *m_p_render_buffer;
	int m_n_buffer_size;

	CMWEncoder *m_p_encoder;
	vector<mw_venc_platform_t> m_vec_platform;
	vector<mw_venc_gpu_info_t> m_vec_gpu;
	vector<int> m_vec_index;

	unsigned char m_cs_renderer[256];
	int m_n_len_renderer;
	unsigned char m_cs_glvendor[256];
	int m_n_len_glvendor;

	char m_cs_menu_file[16];
	char m_cs_menu_video_capture_param[32];
	char m_cs_menu_encode_param[32];
	char m_cs_menu_start_encode[32];
	char m_cs_menu_stop_encode[32];

	char m_cs_disk_size[256];

	char m_cs_menu_device[16];

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

	char m_cs_amd_mem[128];

	char m_cs_encoding_adapter[1024];
	wchar_t m_wcs_file_name[1024];
	wchar_t m_wcs_file_name_main[1024];
	wchar_t m_wcs_file_name_suffixes[16];
	int m_n_file_name_index;
	wchar_t m_wcs_file_name_sub[1024];
	wchar_t m_wcs_dir[256];
	FILE *m_p_file;

	char *m_p_encode_buffer;

	uint32_t m_u32_frametobe_encode;

	int32_t m_i32_frame_ignore_cnt;

	CRITICAL_SECTION m_cs_lock;
};

#endif
