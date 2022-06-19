#ifndef MW_HDR_CAPTURE_UI_H
#define MW_HDR_CAPTURE_UI_H

#include "mw_hdr_capture_ui_manager.h"
#include "../mw_capture/mw_capture.h"
#include "../mw_opengl_render/mw_opengl_render.h"
#include "../mw_encoder/mw_hdr_encoder.h"
#include "LibMWMp4/mw_mp4.h"

#include "../hevcparse/hevc_sei.h"
#include "../hevcparse/hevc_sps.h"
#include "../hevcparse/hevc_nalu.h"

#include "windows.h"
#include "commdlg.h"
#include "time.h"
#include "stdint.h"

#ifndef DFT_WINDOW_WIDTH
#define DFT_WINDOW_WIDTH	1280
#endif
#ifndef DFT_WINDOW_HEIGHT
#define DFT_WINDOW_HEIGHT	720
#endif

enum mw_capture_status_e {
	MW_CAPTUREING,
	MW_NOT_CAPTUREING,
	MW_CAPTURE_ERROR
};

enum mw_encode_status_e {
	MW_ENCODEING,
	MW_NOT_ENCODING,
	MW_ENCODE_ERROR,
	MW_ENCODE_ERROR_NVIDIA,
	MW_ENCODE_STOPERROR
};

enum mw_hdr_preview_mode_e{
	MW_HDR_PREVIEW_AUTO,
	MW_HDR_PREVIEW_HDR,
	MW_HDR_PREVIEW_SDR
};

typedef struct mw_disk_size_s{
	uint32_t		m_u32_gb;
	uint32_t		m_u32_mb;
	uint32_t		m_u32_kb;
	uint32_t		m_u32_b;
}mw_disk_size_t;

class CMWHDRCaptureUI{
public:
	static CMWHDRCaptureUI* create_instance();

	static CMWHDRCaptureUI* get_ui();

	void release_ui();

	bool setup_window(char* t_cs_name);

	void show();

	void cleanup_window();

protected:
	void render_ui();

public:
	static void refresh_cb(GLFWwindow* t_p_win);

	// device clicked
	static void callback_menu_item_selected(
		char* t_cs_menu, 
		char* t_cs_menu_item,
		int t_n_index, 
		void* t_p_param
		){
			if (t_p_param == NULL)
				return;

			CMWHDRCaptureUI* t_p_ui = (CMWHDRCaptureUI*)t_p_param;
			t_p_ui->callback_menu_item_selected_proc(t_cs_menu,
				t_cs_menu_item,
				t_n_index);
	}

	void callback_menu_item_selected_proc(
		char* t_cs_menu,
		char* t_cs_menu_item,
		int t_n_index
		);

	// video capture
	static void callback_video_capture(
		BYTE* pbFrame,
		int cbFrame,
		UINT64 u64TimeStamp,
		void* pParam) {
			if (pParam == NULL)
				return;

			CMWHDRCaptureUI* t_p_ui = (CMWHDRCaptureUI*)pParam;
			t_p_ui->callback_video_capture_proc(pbFrame, cbFrame, u64TimeStamp);
	}

	void callback_video_capture_proc(
		BYTE* pbFrame,
		int cbFrame,
		UINT64 u64TimeStamp);

	static void callback_hdr_value_change(cmw_hdr_param_s t_param,void* t_p_param)
	{
		if(t_p_param == NULL)
			return;

		CMWHDRCaptureUI* t_p_ui = (CMWHDRCaptureUI*)t_p_param;
		t_p_ui->callback_hdr_value_change_proc(t_param);
	}

	void callback_hdr_value_change_proc(cmw_hdr_param_s t_param);

	static DWORD WINAPI thread_cmd_process(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if(lpThreadParameter!=NULL){
			CMWHDRCaptureUI* t_p_this = (CMWHDRCaptureUI*)lpThreadParameter;
			return t_p_this->thread_cmd_process_proc();
		}

		return t_dw_ret;
	}

	DWORD thread_cmd_process_proc();

	static DWORD WINAPI thread_encode(LPVOID lpThreadParameter)
	{
		DWORD t_dw_ret = 0;

		if(lpThreadParameter!=NULL){
			CMWHDRCaptureUI* t_p_this = (CMWHDRCaptureUI*)lpThreadParameter;
			return t_p_this->thread_encode_proc();
		}

		return t_dw_ret;
	}

	DWORD thread_encode_proc();

	// encode output callback
	static void callback_encode(void * user_ptr, const uint8_t * p_frame, uint32_t frame_len, mw_venc_frame_info_t *p_frame_info)
	{
		if (user_ptr == NULL)
			return;

		CMWHDRCaptureUI* t_p_ui = (CMWHDRCaptureUI*)user_ptr;
		t_p_ui->callback_encode_proc(p_frame, frame_len, p_frame_info);
	}

	void callback_encode_proc(const uint8_t * p_frame, uint32_t frame_len, mw_venc_frame_info_t *p_frame_info);
protected:
	mw_render_fourcc_t get_render_fmt_from_fourcc(DWORD t_dw_fourcc);

	void enum_encoder();

	void set_capture_status_text(char* t_cs_text,mw_capture_status_e t_status);
	void set_encode_status_text(char* t_cs_text,mw_encode_status_e t_status);

	unsigned char* setup_for_hdr(unsigned char **t_ppuc_stream_buf, unsigned int *t_pui_stream_len);

	bool get_disk_size(mw_disk_size_t& t_size);

private:
	CMWHDRCaptureUI();
	~CMWHDRCaptureUI();

private:
	static CMWHDRCaptureUI*		s_ui;
	static uint32_t				s_u32_ref;

	static GLFWwindow*			s_p_window;
	static HWND					s_h_window;

	ImGuiContext*				m_p_imgui_ctx;

	CMWHDRCaptureUIManager*		m_p_ui_manager;
	CMWHDRCaptureMenuBar*		m_p_menubar;
	CMWHDRParam*				m_p_hdr_param;
	CMWEncoderAdapter*			m_p_encoder_adapter;
	CMWHDROverlatText*			m_p_overlay_capture;

	CMWCapture*					m_p_capture;
	cmw_video_capture_param_s	m_video_capture_param;
	int							m_n_capture_index;
	int							m_n_ui_select;

	CMWOpenGLRender*			m_p_opengl_render;
	int							m_n_data_size;

	unsigned char*				m_p_yuy2_data;
	CMWRenderBuffer*			m_p_render_buffer;
	int							m_n_buffer_size;

	char						m_cs_menu_file[16];
	char						m_cs_menu_start_record[32];
	char						m_cs_menu_stop_record[32];
	char						m_cs_encoder_adapter[32];

	// dialog for file
	OPENFILENAME				m_ofn;
	HANDLE						m_h_thread_cmd_proc;
	HANDLE						m_h_event_openfile;
	HANDLE						m_h_event_stop_record;
	HANDLE						m_h_event_device_changed;
	HANDLE						m_h_event_exit_thread;

	HANDLE						m_h_thread_encode;
	HANDLE						m_h_event_encode;
	HANDLE						m_h_event_exit_thread_encode;

	char						m_cs_menu_hdr_setting[16];
	char						m_cs_menu_preview_hdr[32];
	char						m_cs_menu_preview_sdr[32];
	char						m_cs_menu_preview_auto[32];
	char						m_cs_menu_hdr_param[32];

	char						m_cs_disk_size[256];

	mw_hdr_preview_mode_e		m_hdr_preview_mode;

	mw_render_ctrl_ex_t			m_render_ctrl_ex;

	char						m_cs_menu_device[16];

	float						m_f_capture_fps;
	int							m_n_capture_num;
	char						m_cs_capture_status[512];
	clock_t						m_last_capture_clock;
	uint64_t					m_u64_total_capture_num;
	mw_capture_status_e			m_capture_status;

	uint32_t					m_u32_frametobe_encode;

	float						m_f_encode_fps;
	int							m_n_encode_num;
	char						m_cs_encode_status[512];
	clock_t						m_last_encode_clock;
	mw_disk_size_t				m_file_size;
	uint64_t					m_u64_total_encode_size;
	uint64_t					m_u64_total_encode_num;
	uint64_t					m_u64_current_file_size;
	mw_encode_status_e			m_encode_status;

	char						m_cs_overlay_text[1024];

	char						m_cs_encoding_adapter[1024];
	wchar_t						m_wcs_file_name[1024];
	wchar_t						m_wcs_file_name_main[1024];
	wchar_t						m_wcs_file_name_suffixes[16];
	int							m_n_file_name_index;
	wchar_t						m_wcs_file_name_sub[1024];
	//char						m_cs_dir[256];
	WCHAR						m_wcs_dir[256];
	clock_t						m_clock_start;

	mw_mp4_handle_t				m_p_file;

	CMWHDREncoder*				m_p_encoder;
	vector<mw_venc_gpu_info_t>	m_vec_gpu;
	vector<int>					m_vec_index;
	mw_venc_param_t				m_venc_param;
	bool						m_b_need_set_mp4;

	unsigned char				m_cs_render[256];
	int							m_n_len_render;

	char*						m_p_encode_buffer;
	uint64_t					m_u64_timestamp;
	CRITICAL_SECTION			m_cs_lock;
};

#endif