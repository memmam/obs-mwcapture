/************************************************************************************************/
// mw_imgui_ui.cpp : implementation of the CMWIMGUIUI class

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

#include "mw_imgui_ui.h"
#include "stdio.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

CMWIMGUIUI *CMWIMGUIUI::s_ui = NULL;
int CMWIMGUIUI::s_n_ref = NULL;
GLFWwindow *CMWIMGUIUI::s_p_window = NULL;
HWND CMWIMGUIUI::s_h_hwnd = NULL;

static void glfw_error_callback(int t_n_error, const char *t_cs_descip)
{
	printf("Glfw Error %d: %s\n", t_n_error, t_cs_descip);
}

CMWIMGUIUI::~CMWIMGUIUI()
{
	if (m_p_capture != NULL) {
		m_p_capture->stop_video_capture();
		m_p_capture->deinit_capture();

		delete m_p_capture;
		m_p_capture = NULL;
	}

	if (m_p_render_buffer != NULL) {
		delete m_p_render_buffer;
		m_p_render_buffer = NULL;
	}

	if (m_p_encoder) {
		delete m_p_encoder;
		m_p_encoder = NULL;
	}

	if (m_h_thread_encode != NULL) {
		SetEvent(m_h_event_exit_thread_encode);
		WaitForSingleObject(m_h_thread_encode, INFINITE);
		CloseHandle(m_h_thread_encode);
		m_h_thread_encode = NULL;
		CloseHandle(m_h_event_encode);
		m_h_event_encode = NULL;
		CloseHandle(m_h_event_exit_thread_encode);
		m_h_event_exit_thread_encode = NULL;
	}

	if (m_h_thread_cmd_proc != NULL) {
		SetEvent(m_h_event_exit_thread);
		WaitForSingleObject(m_h_thread_cmd_proc, INFINITE);
		CloseHandle(m_h_thread_cmd_proc);
		m_h_thread_cmd_proc = NULL;
		CloseHandle(m_h_event_openfile);
		m_h_thread_cmd_proc = NULL;
		CloseHandle(m_h_event_exit_thread);
		m_h_event_exit_thread = NULL;
	}

	mw_venc_deinit();

	if (m_p_encode_buffer != NULL) {
		delete m_p_encode_buffer;
		m_p_encode_buffer = NULL;
	}

	DeleteCriticalSection(&m_cs_lock);
}

CMWIMGUIUI *CMWIMGUIUI::get_ui()
{
	return s_ui;
}

CMWIMGUIUI *CMWIMGUIUI::create_instance()
{
	if (s_ui == 0) {
		s_ui = new CMWIMGUIUI();
		s_n_ref++;
	}

	return s_ui;
}

void CMWIMGUIUI::release_ui()
{
	s_n_ref--;
	if (s_n_ref <= 0) {
		delete s_ui;
		s_ui = 0;
	}
}

bool CMWIMGUIUI::setup_window(char *t_cs_name)
{
	char path[256];
	strcpy_s(path, getenv("HOMEDRIVE"));
	strcat_s(path, getenv("HOMEPATH"));
	strcat_s(path, "\\Magewell");
	_mkdir(path);

	// check support platforms
	uint32_t t_u32_paltforms = mw_venc_get_support_platfrom();
	if (t_u32_paltforms & MW_VENC_PLATFORM_AMD)
		m_vec_platform.push_back(MW_VENC_PLATFORM_AMD);
	if (t_u32_paltforms & MW_VENC_PLATFORM_INTEL)
		m_vec_platform.push_back(MW_VENC_PLATFORM_INTEL);
	if (t_u32_paltforms & MW_VENC_PLATFORM_NVIDIA)
		m_vec_platform.push_back(MW_VENC_PLATFORM_NVIDIA);

	// get adapters
	uint32_t t_u32_gpus = mw_venc_get_gpu_num();
	for (int i = 0; i < t_u32_gpus; i++) {
		mw_venc_gpu_info t_info;
		mw_venc_get_gpu_info_by_index(i, &t_info);
		m_vec_gpu.push_back(t_info);
		m_vec_index.push_back(i);
	}

	bool t_b_ret = false;
	if (s_p_window != NULL) {
		t_b_ret = true;
		return t_b_ret;
	}

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		t_b_ret = false;
		return t_b_ret;
	}

	const char *t_cs_glsl_verison = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// create window with graphics context
	s_p_window = glfwCreateWindow(1280, 720, t_cs_name, NULL, NULL);
	if (s_p_window == NULL) {
		MessageBoxA(
			NULL,
			"This program need OpenGL 3.0 support,please confirm your OpenGL Version",
			"Warning", MB_OK);

		t_b_ret = false;
		return t_b_ret;
	}

	s_h_hwnd = glfwGetWin32Window(s_p_window);

	glfwMakeContextCurrent(s_p_window);
	glfwSwapInterval(1); //enbale vsync

	t_b_ret = gl3wInit() != 0;
	if (t_b_ret) {
		MessageBoxA(NULL, "GL3W ERROR,please confirm your OpenGL",
			    "Warning", MB_OK);
		return t_b_ret;
	}

	const GLubyte *t_p_render_adapter = NULL;
	t_p_render_adapter = glGetString(GL_RENDERER);
	if (t_p_render_adapter == NULL) {
		MessageBoxA(NULL, "Failed to get Adapter", "Warning", MB_OK);
		return false;
	}
	int t_n_len = strlen((const char *)t_p_render_adapter);
	if (t_n_len >= m_n_len_renderer)
		t_n_len = m_n_len_renderer - 1;

	memcpy(m_cs_renderer, t_p_render_adapter, t_n_len);

	const GLubyte *t_p_glvendor = NULL;
	t_p_glvendor = glGetString(GL_VENDOR);
	if (t_p_glvendor == NULL) {
		MessageBoxA(NULL, "Failed to get OpenGL Vendor", "Warning",
			    MB_OK);
		return false;
	}
	t_n_len = strlen((const char *)t_p_glvendor);
	if (t_n_len >= m_n_len_glvendor)
		t_n_len = m_n_len_glvendor - 1;

	memcpy(m_cs_glvendor, t_p_glvendor, t_n_len);

	// setup imgui context
	IMGUI_CHECKVERSION();
	m_p_imgui_context = ImGui::CreateContext();

	// setup imgui style
	ImGui::StyleColorsDark();

	// setup platform/render bindings
	ImGui_ImplGlfw_InitForOpenGL(s_p_window, true);
	ImGui_ImplOpenGL3_Init(t_cs_glsl_verison);

	m_vec4_clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	m_p_ui_manager = new CMWIMGUIUIManager();
	m_p_ui_menubar = new CMWIMGUIWidgetMenuBar(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_ui_menubar);
	m_p_ui_menubar->add_menu(m_cs_menu_file);
	m_p_ui_videoparam = new CMWIMGUIWidgetVideoParam(m_p_ui_manager);
	m_p_ui_menubar->add_menu_submenu(m_cs_menu_file,
					 m_cs_menu_video_capture_param,
					 m_p_ui_videoparam);
	m_p_ui_videoparam->set_param_change_callback(
		callback_video_capture_param_changed, this);
	m_p_ui_menubar->set_menu_sub_menu_enable(
		m_cs_menu_file, m_cs_menu_video_capture_param, false);

	m_p_ui_encodeparam = new CMWIMGUIWidgetEncodeParam(m_p_ui_manager);
	m_p_ui_menubar->add_menu_submenu(m_cs_menu_file, m_cs_menu_encode_param,
					 m_p_ui_encodeparam);
	m_p_ui_menubar->set_menu_sub_menu_enable(m_cs_menu_file,
						 m_cs_menu_encode_param, false);
	m_p_ui_encodeparam->set_enable_encode_callback(callback_enable_encode,
						       this);

	m_p_ui_menubar->add_menu_item(m_cs_menu_file, m_cs_menu_start_encode);
	m_p_ui_menubar->set_menu_item_enable(m_cs_menu_file,
					     m_cs_menu_start_encode, false);
	m_p_ui_menubar->add_menu_item(m_cs_menu_file, m_cs_menu_stop_encode);
	m_p_ui_menubar->set_menu_item_enable(m_cs_menu_file,
					     m_cs_menu_stop_encode, false);

	m_p_ui_menubar->add_menu(m_cs_menu_device);
	m_p_ui_menubar->add_menu_item(m_cs_menu_device,
				      "None - All Devices Supported");
	m_p_ui_menubar->set_menu_item_checked(
		m_cs_menu_device, "None - All Devices Supported", true);
	if (m_p_capture == NULL)
		m_p_capture = new CMWCapture();

	if (m_p_capture != NULL) {
		m_p_capture->refresh_device();
		int t_n_channel_count = m_p_capture->get_channel_count();
		for (int i = 0; i < t_n_channel_count; i++) {
			cmw_channel_info_s t_channel_info;
			bool t_b_info = m_p_capture->get_channel_info(
				i, &t_channel_info);
			if (t_b_info) {
				char t_cs_name[256] = {0};
				if (t_channel_info.m_channel_info.wFamilyID ==
				    MW_FAMILY_ID_USB_CAPTURE) {
					sprintf(t_cs_name, "%s-%s",
						t_channel_info.m_channel_info
							.szProductName,
						t_channel_info.m_channel_info
							.szBoardSerialNo);
				} else {
					sprintf(t_cs_name, "%d-%d %s",
						t_channel_info.m_channel_info
							.byBoardIndex,
						t_channel_info.m_channel_info
							.byChannelIndex,
						t_channel_info.m_channel_info
							.szProductName);
				}
				m_p_ui_menubar->add_menu_item(m_cs_menu_device,
							      t_cs_name);
			}
		}
	}
	m_p_ui_menubar->set_menu_selected_callback(callback_menu_item_selected,
						   this);

	// capture overlay text
	m_p_ui_overlay_capture = new CMWIMGUIWidgetOverlayText(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_ui_overlay_capture);
	m_p_ui_overlay_capture->set_name("capture status");

	cmw_ui_video_capture_param_s t_ui_param =
		m_p_ui_videoparam->get_ui_video_capture_param();
	m_video_capture_param.m_n_width = t_ui_param.m_n_width;
	m_video_capture_param.m_n_height = t_ui_param.m_n_height;
	m_video_capture_param.m_ul_frameduration = t_ui_param.m_n_frameduration;
	m_video_capture_param.m_dw_fourcc = t_ui_param.m_u32_color;

	// init opengl render
	if (m_p_opengl_render == NULL) {
		m_p_opengl_render = new CMWOpenGLRender();
		mw_render_init_t t_init;
		t_init.m_u32_width = m_video_capture_param.m_n_width;
		t_init.m_u32_height = m_video_capture_param.m_n_height;
		t_init.m_mode = get_render_fmt_from_fourcc(
			m_video_capture_param.m_dw_fourcc);
		t_init.m_u8_need_rev = 1;
		m_p_opengl_render->open_sdr(&t_init);
	}

	m_h_event_openfile = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_stop_record = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_device_changed = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_exit_thread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_thread_cmd_proc =
		CreateThread(NULL, 0, thread_cmd_process, this, 0, NULL);

	glfwSetWindowRefreshCallback(s_p_window, refresh_cb);

	m_p_ui_encodeparam->init_support_platform(m_vec_platform, m_vec_gpu,
						  m_vec_index);

	// opengl version
	const GLubyte *t_p_version = glGetString(GL_VERSION);
	printf("opengl version:%s\n", t_p_version);

	return t_b_ret;
}

void CMWIMGUIUI::show()
{
	while (!glfwWindowShouldClose(s_p_window)) {
		// poll and handle events (inputs,window resize,etc.)
		glfwPollEvents();

		render_ui();
	}
}

void CMWIMGUIUI::cleanup_window()
{
	// clean up

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(s_p_window);
	glfwTerminate();
}

void CMWIMGUIUI::render_ui()
{
	// start the imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	int t_n_width, t_n_height;
	glfwGetWindowSize(s_p_window, &t_n_width, &t_n_height);

	memset(m_cs_overlay_text, 0, 1024);
	char t_cs_size[128] = {0};
	int t_n_gb = m_u64_total_encode_size >> 30;
	int t_n_mb = (m_u64_total_encode_size >> 20) & 1023;
	int t_n_kb = (m_u64_total_encode_size >> 10) & 1023;
	int t_n_b = (m_u64_total_encode_size)&1023;
	sprintf(t_cs_size, "%dGB %dMB %dKB %dB", t_n_gb, t_n_mb, t_n_kb, t_n_b);

	if (m_h_event_encode != NULL) {
		if (m_file_size.m_u32_mb != t_n_mb) {
			m_file_size.m_u32_gb = t_n_gb;
			m_file_size.m_u32_mb = t_n_mb;
			m_file_size.m_u32_kb = t_n_kb;
			m_file_size.m_u32_b = t_n_b;
		}

		mw_disk_size_t t_disk_size;
		if (get_disk_size(t_disk_size)) {
			if (t_disk_size.m_u32_gb < 1 &&
			    t_disk_size.m_u32_mb < 200) {
				SetEvent(m_h_event_stop_record);
				MessageBoxA(
					s_h_hwnd,
					"There is not enough space on the disk.Encoding will auto exit.",
					"Warning", MB_OK);
			}

			memset(m_cs_disk_size, 0, 256);
			sprintf(m_cs_disk_size, "%dGB %dMB",
				t_disk_size.m_u32_gb, t_disk_size.m_u32_mb);
		}
	}

	sprintf(m_cs_overlay_text,
		"render adapter:%s\n"
		"encode adapter:%s\n"
		"capture status:%s capture fps:%.2f capture frames:%lld\n"
		"encode status:%s encode fps:%.2f encode frames:%lld\n"
		"encode size:%s\n"
		"disk size:%s\n",
		m_cs_renderer, m_cs_encoding_adapter, m_cs_capture_status,
		m_f_capture_fps, m_u64_total_capture_num, m_cs_encode_status,
		m_f_encode_fps, m_u64_total_encode_num, t_cs_size,
		m_cs_disk_size);
	m_p_ui_overlay_capture->set_text(m_cs_overlay_text);

	m_p_ui_manager->update_ui();

	bool t_b_captureing = m_p_capture->get_is_captureing();
	if (!t_b_captureing) {
		if (!m_p_render_buffer->is_clear())
			m_p_render_buffer->clear_data();
	}

	mw_render_ctrl_t t_ctrl;
	t_ctrl.m_u32_display_w = t_n_width;
	t_ctrl.m_u32_display_h = t_n_height - 18;

	if (m_i32_frame_ignore_cnt == 0)
		m_p_opengl_render->render_sdr(&t_ctrl, !t_b_captureing);
	else
		m_p_opengl_render->render_sdr(&t_ctrl, true);

	//rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(s_p_window);
}

void CMWIMGUIUI::refresh_cb(GLFWwindow *t_p_win)
{
	s_ui->render_ui();
}

void CMWIMGUIUI::callback_menu_item_selected_proc(char *t_cs_menu,
						  char *t_cs_menu_item,
						  int t_n_index)
{
	if (strcmp(t_cs_menu, m_cs_menu_file) == 0) {
		// file clicked
		if (strcmp(t_cs_menu_item, m_cs_menu_start_encode) == 0) {
			SetEvent(m_h_event_openfile);
		} else if (strcmp(t_cs_menu_item, m_cs_menu_stop_encode) == 0) {
			printf("stop encode clicked\n");
			SetEvent(m_h_event_stop_record);
		}
	} else if (strcmp(t_cs_menu, m_cs_menu_device) == 0) {
		// device changed
		m_n_ui_select = t_n_index;
		SetEvent(m_h_event_device_changed);
	} else {
		// error
	}
}

void CMWIMGUIUI::callback_video_capture_proc(BYTE *pbFrame, int cbFrame,
					     UINT64 u64TimeStamp)
{
	// calac real fps
	m_n_capture_num++;
	m_u64_total_capture_num++;
	clock_t t_clock = clock();
	if (m_last_capture_clock == 0)
		m_last_capture_clock = t_clock;
	if (t_clock - m_last_capture_clock >= 1000) {
		m_f_capture_fps = m_n_capture_num * 1000 * 1.0 /
				  (t_clock - m_last_capture_clock);
		m_n_capture_num = 0;
		m_last_capture_clock = t_clock;
	}

	if (m_i32_frame_ignore_cnt > 0)
		m_i32_frame_ignore_cnt--;

	if (m_h_event_encode != NULL) {
		if (TryEnterCriticalSection(&m_cs_lock)) {
			clock_t t_begin = clock();
			memcpy(m_p_encode_buffer, pbFrame, cbFrame);
			clock_t t_end = clock();
			long t_l_duration = t_end - t_begin;
			char t_cs_copy_time[32] = {0};
			sprintf(t_cs_copy_time, "copy:%d %d\n", t_l_duration,
				cbFrame);
			//printf(t_cs_copy_time);
			SetEvent(m_h_event_encode);
			LeaveCriticalSection(&m_cs_lock);
		}
	}
}

void CMWIMGUIUI::callback_video_capture_param_changed_proc(
	cmw_ui_video_capture_param_s t_param)
{
	// wether change
	bool t_b_change = false;
	if (m_video_capture_param.m_n_width != t_param.m_n_width) {
		m_video_capture_param.m_n_width = t_param.m_n_width;
		t_b_change = true;
	}

	if (m_video_capture_param.m_n_height != t_param.m_n_height) {
		m_video_capture_param.m_n_height = t_param.m_n_height;
		t_b_change = true;
	}

	if (m_video_capture_param.m_ul_frameduration !=
	    t_param.m_n_frameduration) {
		m_video_capture_param.m_ul_frameduration =
			t_param.m_n_frameduration;
		t_b_change = true;
	}

	if (m_video_capture_param.m_dw_fourcc != t_param.m_u32_color) {
		m_video_capture_param.m_dw_fourcc = t_param.m_u32_color;
		t_b_change = true;
	}

	if (t_b_change) {
		if (m_p_capture->get_is_captureing()) {
			m_p_capture->stop_video_capture();
			m_capture_status = MW_NOT_CAPTUREING;
			set_capture_status_text(m_cs_capture_status,
						m_capture_status);
			m_i32_frame_ignore_cnt = 8;
		} else {
			m_i32_frame_ignore_cnt = 4;
		}
	} else {
		if (m_p_capture->get_is_captureing()) {
			m_capture_status = MW_CAPTUREING;
			set_capture_status_text(m_cs_capture_status,
						m_capture_status);
			return;
		}
	}

	m_p_opengl_render->close();
	mw_render_init_t t_render_init;
	t_render_init.m_u32_width = m_video_capture_param.m_n_width;
	t_render_init.m_u32_height = m_video_capture_param.m_n_height;
	t_render_init.m_mode =
		get_render_fmt_from_fourcc(m_video_capture_param.m_dw_fourcc);
	t_render_init.m_u8_need_rev = 1;

	int t_n_stride = FOURCC_CalcMinStride(m_video_capture_param.m_dw_fourcc,
					      m_video_capture_param.m_n_width,
					      4);
	m_n_data_size = FOURCC_CalcImageSize(m_video_capture_param.m_dw_fourcc,
					     m_video_capture_param.m_n_width,
					     m_video_capture_param.m_n_height,
					     t_n_stride);

	m_n_buffer_size = m_video_capture_param.m_n_width *
			  m_video_capture_param.m_n_height * 4;

	cmw_video_base_s t_video_base;
	t_video_base.m_n_width = m_video_capture_param.m_n_width;
	t_video_base.m_n_height = m_video_capture_param.m_n_height;
	t_video_base.m_vf_fourcc =
		get_vf_fourcc_from_fourcc(m_video_capture_param.m_dw_fourcc);
	t_video_base.m_n_fps_den = 1;
	t_video_base.m_n_fps_num =
		10000000 / m_video_capture_param.m_ul_frameduration;
	m_p_ui_encodeparam->set_video_base(t_video_base);

	m_f_capture_fps = 0.0;
	m_n_capture_num = 0;
	m_last_capture_clock = 0;
	memset(m_cs_capture_status, 0, 512);
	m_u64_total_capture_num = 0;

	m_p_opengl_render->open_sdr(&t_render_init);
	CMWRenderBuffer *t_p_render_buffer = NULL;
	t_p_render_buffer = m_p_opengl_render->get_render_buffer();

	if (m_n_capture_index != -1) {
		bool t_b_capture = m_p_capture->start_video_capture(
			m_video_capture_param, callback_video_capture, this,
			t_p_render_buffer);
		if (t_b_capture) {
			if (m_vec_platform.size() <= 0) {
				m_p_ui_menubar->set_menu_sub_menu_enable(
					m_cs_menu_file, m_cs_menu_encode_param,
					false);
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_start_encode,
					false);
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_stop_encode,
					false);
			} else {
				m_p_ui_menubar->set_menu_sub_menu_enable(
					m_cs_menu_file, m_cs_menu_encode_param,
					true);
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_start_encode,
					true);
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_stop_encode,
					false);
			}

			m_capture_status = MW_CAPTUREING;
			set_capture_status_text(m_cs_capture_status,
						m_capture_status);
		} else {
			m_capture_status = MW_CAPTURE_ERROR;
			set_capture_status_text(m_cs_capture_status,
						m_capture_status);

			m_p_ui_menubar->set_menu_sub_menu_enable(
				m_cs_menu_file, m_cs_menu_encode_param, false);
			m_p_ui_menubar->set_menu_item_enable(
				m_cs_menu_file, m_cs_menu_start_encode, false);
			m_p_ui_menubar->set_menu_item_enable(
				m_cs_menu_file, m_cs_menu_stop_encode, false);
		}
	} else {
		m_p_render_buffer->allocate_buffer(m_n_buffer_size);

		m_p_ui_menubar->set_menu_sub_menu_enable(
			m_cs_menu_file, m_cs_menu_encode_param, false);
		m_p_ui_menubar->set_menu_item_enable(
			m_cs_menu_file, m_cs_menu_start_encode, false);
		m_p_ui_menubar->set_menu_item_enable(
			m_cs_menu_file, m_cs_menu_stop_encode, false);

		m_capture_status = MW_NOT_CAPTUREING;
		set_capture_status_text(m_cs_capture_status, m_capture_status);
	}
}

void CMWIMGUIUI::callback_encode_proc(const uint8_t *p_frame,
				      uint32_t frame_len,
				      mw_venc_frame_info_t *p_frame_info)
{
	m_n_encode_num++;
	m_u64_total_encode_num++;
	m_u64_total_encode_size += frame_len;

	clock_t t_clock = clock();
	if (m_last_encode_clock == 0)
		m_last_encode_clock = t_clock;

	if (t_clock - m_last_encode_clock >= 1000) {
		m_f_encode_fps = m_n_encode_num * 1000 * 1.0 /
				 (t_clock - m_last_encode_clock);
		m_n_encode_num = 0;
		m_last_encode_clock = t_clock;
	}

	if (m_p_file != NULL) {
		// when file is larger than 1GB than split the file
		if (m_u64_current_file_size > 1024 * 1024 * 1024 &&
		    p_frame_info->frame_type == MW_VENC_FRAME_TYPE_IDR) {
			fclose(m_p_file);
			m_p_file = NULL;
			m_u64_current_file_size = 0;

			m_n_file_name_index++;
			memset(m_wcs_file_name_sub, 0, 1024 * 2);
			wsprintf(m_wcs_file_name_sub, L"%s-%d%s",
				 m_wcs_file_name_main, m_n_file_name_index,
				 m_wcs_file_name_suffixes);
			m_p_file = _wfopen(m_wcs_file_name_sub, L"wb");
		}

		if (m_p_file != NULL) {
			fwrite(p_frame, 1, frame_len, m_p_file);
			m_u64_current_file_size += frame_len;
		}
	}
}

DWORD CMWIMGUIUI::thread_cmd_process_proc()
{
	DWORD t_dw_re = 0;

	m_u32_frametobe_encode = 0;

	HANDLE t_array_events[4] = {m_h_event_openfile, m_h_event_stop_record,
				    m_h_event_device_changed,
				    m_h_event_exit_thread};
	while (true) {
		DWORD t_dw_ret = WaitForMultipleObjects(4, t_array_events,
							false, INFINITE);
		if (t_dw_ret == WAIT_OBJECT_0) {
			// show file dialog

			mw_venc_param_t t_param =
				m_p_ui_encodeparam->get_encode_param();
			mw_venc_gpu_info_t t_gpu =
				m_p_ui_encodeparam->get_encode_adapter();
			int t_n_gpu_index =
				m_p_ui_encodeparam->get_encoder_index();

			tm *t_p_ltime = NULL;
			time_t t_tt = time(NULL);
			t_p_ltime = localtime(&t_tt);

			char t_cs_path[1024] = {0};
			strcpy_s(t_cs_path, getenv("HOMEDRIVE"));
			strcat_s(t_cs_path, getenv("HOMEPATH"));
			strcat_s(t_cs_path, "\\Magewell");
			WCHAR t_wcs_path[1024] = {0};
			int nLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED,
						       t_cs_path, -1, NULL, 0);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, t_cs_path,
					    -1, t_wcs_path, nLen);

			WCHAR t_szFileName[MAX_PATH] = {0};
			ZeroMemory(&m_ofn, sizeof(m_ofn));
			if (t_param.code_type == MW_VENC_CODE_TYPE_H264) {
				wsprintf(t_szFileName,
					 L"%d%02d%02d-%02d_%02d_%02d.h264",
					 t_p_ltime->tm_year + 1900,
					 t_p_ltime->tm_mon + 1,
					 t_p_ltime->tm_mday, t_p_ltime->tm_hour,
					 t_p_ltime->tm_min, t_p_ltime->tm_sec);
				m_ofn.lpstrFilter =
					_T("H264 Files (*.h264)\0*.h264\0");
				m_ofn.lpstrDefExt = _T("H264 Files");
			} else {
				wsprintf(t_szFileName,
					 L"%d%02d%02d-%02d_%02d_%02d.h265",
					 t_p_ltime->tm_year + 1900,
					 t_p_ltime->tm_mon + 1,
					 t_p_ltime->tm_mday, t_p_ltime->tm_hour,
					 t_p_ltime->tm_min, t_p_ltime->tm_sec);
				m_ofn.lpstrFilter =
					_T("H265 Files (*.h265)\0*.h265\0");
				m_ofn.lpstrDefExt = _T("H265 Files");
			}

			m_ofn.lStructSize = sizeof(m_ofn);
			m_ofn.hwndOwner = s_h_hwnd;

			m_ofn.lpstrFile = t_szFileName;
			m_ofn.nMaxFile = MAX_PATH;
			m_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
				      OFN_HIDEREADONLY;

			m_ofn.lpstrInitialDir = t_wcs_path;

			BOOL t_b_re = GetSaveFileName(&m_ofn);
			if (t_b_re) {
				// start encode

				memset(m_wcs_file_name, 0, 1024 * 2);
				memset(m_wcs_file_name_main, 0, 1024 * 2);
				memset(m_wcs_file_name_sub, 0, 1024 * 2);
				memset(m_wcs_file_name_suffixes, 0, 1024 * 2);
				m_n_file_name_index = 0;

				wsprintf(m_wcs_file_name, L"%s",
					 m_ofn.lpstrFile);

				int t_n_len_file_name = wcslen(m_wcs_file_name);
				memcpy(m_wcs_file_name_sub, m_wcs_file_name,
				       t_n_len_file_name * 2);

				wchar_t *t_p_dirpos = NULL;
				t_p_dirpos = wcsstr(m_wcs_file_name, L":");
				int t_n_len = 0;
				if (t_p_dirpos != NULL) {
					t_n_len = t_p_dirpos - m_wcs_file_name;
					memset(m_wcs_dir, 0, 256 * 2);
					memcpy(m_wcs_dir, m_wcs_file_name,
					       (t_n_len + 1) * 2);

					// check disk size
					mw_disk_size_t t_disk_size;
					if (get_disk_size(t_disk_size)) {
						if (t_disk_size.m_u32_gb < 1 &&
						    t_disk_size.m_u32_mb <
							    200) {
							MessageBoxA(
								s_h_hwnd,
								"There is not enough space on the disk.",
								"Warning",
								MB_OK);
							memset(m_cs_disk_size,
							       0, 256);
							sprintf(m_cs_disk_size,
								"not selected");
							continue;
						}
					}

					memset(m_cs_disk_size, 0, 256);
					sprintf(m_cs_disk_size, "%dGB %dMB",
						t_disk_size.m_u32_gb,
						t_disk_size.m_u32_mb);
				}

				wchar_t *t_p_h264_pos = NULL;
				t_p_h264_pos =
					wcsstr(m_wcs_file_name, L".h264");
				wchar_t *t_p_h265_pos = NULL;
				t_p_h265_pos =
					wcsstr(m_wcs_file_name, L".h265");
				t_n_len = 0;
				if (t_p_h264_pos != NULL) {
					t_n_len =
						t_p_h264_pos - m_wcs_file_name;
					wsprintf(m_wcs_file_name_suffixes,
						 L".h264");
				} else if (t_p_h265_pos != NULL) {
					t_n_len =
						t_p_h265_pos - m_wcs_file_name;
					wsprintf(m_wcs_file_name_suffixes,
						 L".h265");
				}

				memcpy(m_wcs_file_name_main, m_wcs_file_name,
				       t_n_len * 2);

				m_f_encode_fps = 0.0;
				m_n_encode_num = 0;
				m_last_encode_clock = 0;
				memset(m_cs_encode_status, 0, 512);
				m_u64_total_encode_size = 0;
				m_u64_total_encode_num = 0;
				m_u64_current_file_size = 0;

				m_p_file = _wfopen(m_wcs_file_name, L"wb");

				bool t_b_encoding = false;
				if (m_p_encoder != NULL)
					t_b_encoding =
						m_p_encoder->open_encoder(
							t_gpu.platform,
							&t_param,
							callback_encode, this,
							t_n_gpu_index);

				if (t_b_encoding) {
					m_p_ui_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_start_encode, false);
					m_p_ui_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_stop_encode, true);
					m_p_ui_menubar->set_menu_sub_menu_enable(
						m_cs_menu_file,
						m_cs_menu_video_capture_param,
						false);
					m_p_ui_menubar->set_menu_sub_menu_enable(
						m_cs_menu_file,
						m_cs_menu_encode_param, false);
					m_p_ui_menubar->set_menu_enable(
						m_cs_menu_device, false);

					if (t_gpu.platform ==
					    MW_VENC_PLATFORM_AMD) {
						get_amd_mem(
							t_param.amd_mem_reserved);
						sprintf(m_cs_encoding_adapter,
							"%s mem:%s",
							t_gpu.gpu_name,
							m_cs_amd_mem);
					} else
						sprintf(m_cs_encoding_adapter,
							"%s", t_gpu.gpu_name);

					m_encode_status = MW_ENCODEING;
					set_encode_status_text(
						m_cs_encode_status,
						m_encode_status);

					// create thread
					m_h_thread_encode = NULL;
					m_h_event_encode = NULL;
					m_h_event_exit_thread_encode = NULL;
					m_h_event_encode = CreateEvent(
						NULL, FALSE, FALSE, NULL);
					m_h_event_exit_thread_encode =
						CreateEvent(NULL, FALSE, FALSE,
							    NULL);
					m_h_thread_encode = CreateThread(
						NULL, 0, thread_encode, this, 0,
						NULL);

				} else {
					if (t_gpu.platform ==
						    MW_VENC_PLATFORM_NVIDIA &&
					    t_param.fourcc ==
						    MW_VENC_FOURCC_YUY2)
						m_encode_status =
							MW_ENCODE_ERROR_NVIDIA;
					else
						m_encode_status =
							MW_ENCODE_ERROR;

					set_encode_status_text(
						m_cs_encode_status,
						m_encode_status);
					m_p_ui_menubar->set_menu_item_checked(
						m_cs_menu_file,
						m_cs_menu_start_encode, false);
					m_p_ui_menubar->set_menu_enable(
						m_cs_menu_device, true);

					// delete file
					if (m_p_file != NULL) {
						fclose(m_p_file);
						_wunlink(m_wcs_file_name);
						_wremove(m_wcs_file_name);
						m_p_file = NULL;
					}

					MessageBoxA(
						s_h_hwnd,
						"This adapter doesn't support this param",
						"Warning", MB_OK);
				}
			}
		} else if (t_dw_ret == WAIT_OBJECT_0 + 1) {
			if (m_h_thread_encode != NULL) {
				SetEvent(m_h_event_exit_thread_encode);
				WaitForSingleObject(m_h_thread_encode,
						    INFINITE);
				CloseHandle(m_h_thread_encode);
				m_h_thread_encode = NULL;
				CloseHandle(m_h_event_encode);
				m_h_event_encode = NULL;
				CloseHandle(m_h_event_exit_thread_encode);
				m_h_event_exit_thread_encode = NULL;
			}

			EnterCriticalSection(&m_cs_lock);

			memset(m_cs_encoding_adapter, 0, 1024);

			m_p_encoder->set_is_encoding(false);
			bool t_b_closed = false;
			if (m_p_encoder != NULL)
				t_b_closed = m_p_encoder->close_encoder();

			if (m_p_file != NULL) {
				fclose(m_p_file);
				m_p_file = NULL;
			}

			LeaveCriticalSection(&m_cs_lock);

			if (t_b_closed) {
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_start_encode,
					true);
				m_p_ui_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_stop_encode,
					false);
				m_p_ui_menubar->set_menu_sub_menu_enable(
					m_cs_menu_file,
					m_cs_menu_video_capture_param, true);
				m_p_ui_menubar->set_menu_sub_menu_enable(
					m_cs_menu_file, m_cs_menu_encode_param,
					true);
				m_p_ui_menubar->set_menu_enable(
					m_cs_menu_device, true);

				m_encode_status = MW_NOT_ENCODING;
				set_encode_status_text(m_cs_encode_status,
						       m_encode_status);
			} else {
				m_encode_status = MW_ENCODE_STOPERROR;
				set_encode_status_text(m_cs_encode_status,
						       m_encode_status);
				m_p_ui_menubar->set_menu_item_checked(
					m_cs_menu_file, m_cs_menu_start_encode,
					false);
				m_p_ui_menubar->set_menu_enable(
					m_cs_menu_device, false);
			}
		} else if (t_dw_ret == WAIT_OBJECT_0 + 2) {
			m_p_capture->stop_video_capture();
			m_p_capture->close_channel();

			m_capture_status = MW_NOT_CAPTUREING;
			set_capture_status_text(m_cs_capture_status,
						m_capture_status);

			if (m_n_ui_select == 0) {
				m_n_capture_index = -1;
				continue;
			}

			bool t_b_open = false;
			t_b_open = m_p_capture->open_channel(m_n_ui_select - 1);
			if (!t_b_open) {
				m_capture_status = MW_CAPTURE_ERROR;
				set_capture_status_text(m_cs_capture_status,
							m_capture_status);
				m_n_capture_index = -1;
				continue;
			}

			MW_FAMILY_ID t_family_id = m_p_capture->get_family_id();

			vector<uint32_t> t_vec_format =
				m_p_capture->get_supported_colorformat();
			m_p_ui_videoparam->set_supported_colorformat(
				t_vec_format, t_family_id);

			vector<MWSIZE> t_vec_resolution =
				m_p_capture->get_supported_resolution();
			m_p_ui_videoparam->set_supported_resolution(
				t_vec_resolution, t_family_id);

			vector<uint64_t> t_vec_duration =
				m_p_capture->get_supported_frameduration();
			m_p_ui_videoparam->set_supported_frameduration(
				t_vec_duration, t_family_id);

			// update video param menu
			m_p_ui_menubar->set_menu_sub_menu_enable(
				m_cs_menu_file, m_cs_menu_video_capture_param,
				true);
			m_p_ui_menubar->set_menu_sub_menu_enable(
				m_cs_menu_file, m_cs_menu_encode_param, false);
			m_p_ui_menubar->set_menu_item_enable(
				m_cs_menu_file, m_cs_menu_start_encode, false);
			m_p_ui_menubar->set_menu_item_enable(
				m_cs_menu_file, m_cs_menu_stop_encode, false);
			m_n_capture_index = m_n_ui_select - 1;
		} else if (t_dw_ret == WAIT_OBJECT_0 + 3) {
			// exit thread
			t_dw_re = 0;
			break;
		} else {
			//exit thread with error
			t_dw_ret = 1;
			break;
		}
	}

	return t_dw_re;
}

DWORD CMWIMGUIUI::thread_encode_proc()
{
	DWORD t_dw_ret = 0;

	HANDLE t_array_handle[] = {m_h_event_encode,
				   m_h_event_exit_thread_encode};

	bool t_b_running = true;
	while (t_b_running) {
		DWORD t_dw_obj = WaitForMultipleObjects(2, t_array_handle,
							FALSE, INFINITE);
		if (t_dw_obj == WAIT_OBJECT_0) {
			if (m_p_encoder != NULL) {
				long t_l_duration = 0;
				EnterCriticalSection(&m_cs_lock);
				if (m_p_encoder->get_is_encoding()) {
					clock_t t_begin = clock();
					m_p_encoder->put_frame(
						(uint8_t *)m_p_encode_buffer);
					clock_t t_end = clock();
					t_l_duration = t_end - t_begin;
				}
				LeaveCriticalSection(&m_cs_lock);
				char t_cs_encode_time[16] = {0};
				sprintf(t_cs_encode_time, "encode:%ld\n",
					t_l_duration);
				//printf(t_cs_encode_time);
			}
		} else if (t_dw_obj == WAIT_OBJECT_0 + 1) {
			t_b_running = false;
			break;
		} else {
			t_b_running = false;
			break;
		}
	}

	return t_dw_ret;
}

void CMWIMGUIUI::callback_enable_encode_proc(bool t_b_enable)
{
	m_p_ui_menubar->set_menu_item_enable(
		m_cs_menu_file, m_cs_menu_start_encode, t_b_enable);
}

DWORD CMWIMGUIUI::get_fourcc_from_str(char *t_cs_color)
{
	DWORD t_dw_color = MWFOURCC_UNK;

	if (strcmp(t_cs_color, "NV12") == 0)
		t_dw_color = MWFOURCC_NV12;
	else if (strcmp(t_cs_color, "NV21") == 0)
		t_dw_color = MWFOURCC_NV21;
	else if (strcmp(t_cs_color, "YV12") == 0)
		t_dw_color = MWFOURCC_YV12;
	else if (strcmp(t_cs_color, "I420") == 0)
		t_dw_color = MWFOURCC_I420;
	else if (strcmp(t_cs_color, "YUY2") == 0)
		t_dw_color = MWFOURCC_YUY2;
	else if (strcmp(t_cs_color, "P010") == 0)
		t_dw_color = MWFOURCC_P010;
	else if (strcmp(t_cs_color, "BGRA") == 0)
		t_dw_color = MWFOURCC_BGRA;
	else if (strcmp(t_cs_color, "RGBA") == 0)
		t_dw_color = MWFOURCC_RGBA;
	else if (strcmp(t_cs_color, "ARGB") == 0)
		t_dw_color = MWFOURCC_ARGB;
	else if (strcmp(t_cs_color, "ABGR") == 0)
		t_dw_color = MWFOURCC_ABGR;

	return t_dw_color;
}

mw_render_fourcc_t CMWIMGUIUI::get_render_fmt_from_fourcc(DWORD t_dw_fourcc)
{
	mw_render_fourcc_t t_color = MW_RENDER_FOURCC_CNT;

	switch (t_dw_fourcc) {
	case MWFOURCC_NV12:
		t_color = MW_RENDER_FOURCC_NV12;
		break;
	case MWFOURCC_NV21:
		t_color = MW_RENDER_FOURCC_NV21;
		break;
	case MWFOURCC_YV12:
		t_color = MW_RENDER_FOURCC_YV12;
		break;
	case MWFOURCC_I420:
		t_color = MW_RENDER_FOURCC_I420;
		break;
	case MWFOURCC_YUY2:
		t_color = MW_RENDER_FOURCC_YUY2;
		break;
	case MWFOURCC_P010:
		t_color = MW_RENDER_FOURCC_P010;
		break;
	case MWFOURCC_BGRA:
		t_color = MW_RENDER_FOURCC_BGRA;
		break;
	case MWFOURCC_RGBA:
		t_color = MW_RENDER_FOURCC_RGBA;
		break;
	case MWFOURCC_ARGB:
		t_color = MW_RENDER_FOURCC_ARGB;
		break;
	case MWFOURCC_ABGR:
		t_color = MW_RENDER_FOURCC_ABGR;
		break;
	default:
		break;
	}

	return t_color;
}

mw_venc_fourcc_t CMWIMGUIUI::get_vf_fourcc_from_fourcc(DWORD t_dw_fourcc)
{
	mw_venc_fourcc_t t_fourcc = MW_VENC_FOURCC_YUY2;

	switch (t_dw_fourcc) {
	case MWFOURCC_NV12:
		t_fourcc = MW_VENC_FOURCC_NV12;
		break;
	case MWFOURCC_NV21:
		t_fourcc = MW_VENC_FOURCC_NV21;
		break;
	case MWFOURCC_YV12:
		t_fourcc = MW_VENC_FOURCC_YV12;
		break;
	case MWFOURCC_I420:
		t_fourcc = MW_VENC_FOURCC_I420;
		break;
	case MWFOURCC_YUY2:
		t_fourcc = MW_VENC_FOURCC_YUY2;
		break;
	case MWFOURCC_P010:
		t_fourcc = MW_VENC_FOURCC_P010;
		break;
	case MWFOURCC_BGRA:
		t_fourcc = MW_VENC_FOURCC_BGRA;
		break;
	case MWFOURCC_RGBA:
		t_fourcc = MW_VENC_FOURCC_RGBA;
		break;
	case MWFOURCC_ARGB:
		t_fourcc = MW_VENC_FOURCC_ARGB;
		break;
	case MWFOURCC_ABGR:
		t_fourcc = MW_VENC_FOURCC_ABGR;
		break;
	default:
		break;
	}

	return t_fourcc;
}

void CMWIMGUIUI::set_capture_status_text(char *t_cs_text,
					 mw_capture_status_e t_status)
{
	switch (t_status) {
	case MW_CAPTUREING:
		sprintf(t_cs_text, "video capturing");
		break;
	case MW_NOT_CAPTUREING:
		sprintf(t_cs_text, "video capture stopped");
		break;
	case MW_CAPTURE_ERROR:
		sprintf(t_cs_text, "video capture error");
		break;
	default:
		sprintf(t_cs_text, "video capture error");
		break;
	}
}

void CMWIMGUIUI::set_encode_status_text(char *t_cs_text,
					mw_encode_status_e t_status)
{
	memset(t_cs_text, 0, 256);
	switch (t_status) {
	case MW_ENCODEING:
		sprintf(t_cs_text, "encode capturing");
		break;
	case MW_NOT_ENCODING:
		sprintf(t_cs_text, "encode stopped");
		break;
	case MW_ENCODE_ERROR:
		sprintf(t_cs_text,
			"This graphic adapter doesn't support current encode param,\nplease try with another param.");
		break;
	case MW_ENCODE_ERROR_NVIDIA:
		sprintf(t_cs_text, "NVIDIA doesn't support YUY2 encoding.");
		break;
	case MW_ENCODE_STOPERROR:
		sprintf(t_cs_text, "encode stop error");
		break;
	default:
		sprintf(t_cs_text, "encode error");
		break;
	}
}

void CMWIMGUIUI::get_amd_mem(mw_venc_amd_mem_type t_type)
{
	memset(m_cs_amd_mem, 0, 128);

	switch (t_type) {
	case MW_VENC_AMD_MEM_AUTO:
		sprintf(m_cs_amd_mem, "auto");
		break;
	case MW_VENC_AMD_MEM_DX11:
		sprintf(m_cs_amd_mem, "dx11");
		break;
	case MW_VENC_AMD_MEM_DX9:
		sprintf(m_cs_amd_mem, "dx9");
		break;
	case MW_VENC_AMD_MEM_CPU:
		sprintf(m_cs_amd_mem, "host");
		break;
	default:
		break;
	}
}

bool CMWIMGUIUI::get_disk_size(mw_disk_size_t &t_size)
{
	bool t_b_ret = false;

	ULARGE_INTEGER t_free_bytes_available_to_caller = {0};
	ULARGE_INTEGER t_total_number_of_bytes = {0};
	ULARGE_INTEGER t_total_number_of_free_bytes = {0};

	t_b_ret = GetDiskFreeSpaceEx(m_wcs_dir,
				     &t_free_bytes_available_to_caller,
				     &t_total_number_of_bytes,
				     &t_total_number_of_free_bytes);
	if (t_b_ret) {
		uint64_t t_u64_freebytes_avaiable_to_user =
			t_free_bytes_available_to_caller.QuadPart;
		uint64_t t_u64_total_number_bytes =
			t_total_number_of_bytes.QuadPart;
		uint64_t t_u64_total_number_of_free_bytes =
			t_total_number_of_free_bytes.QuadPart;

		t_size.m_u32_b = t_u64_freebytes_avaiable_to_user & 1023;
		t_size.m_u32_kb = (t_u64_freebytes_avaiable_to_user >> 10) &
				  1023;
		t_size.m_u32_mb = (t_u64_freebytes_avaiable_to_user >> 20) &
				  1023;
		t_size.m_u32_gb = (t_u64_freebytes_avaiable_to_user >> 30);
	}

	return t_b_ret;
}

CMWIMGUIUI::CMWIMGUIUI()
{
	m_p_imgui_context = NULL;
	m_p_ui_manager = NULL;
	m_p_ui_menubar = NULL;
	m_p_ui_videoparam = NULL;
	m_p_ui_encodeparam = NULL;

	m_p_opengl_render = NULL;

	m_u32_frametobe_encode = 0;

	m_n_len_renderer = 256;
	memset(m_cs_renderer, 0, m_n_len_renderer);

	m_n_len_glvendor = 256;
	memset(m_cs_glvendor, 0, m_n_len_glvendor);

	memset(m_cs_disk_size, 0, 256);
	sprintf(m_cs_disk_size, "not selected");

	// init mw_venc
	mw_venc_init();

	// create  encoder
	m_p_encoder = new CMWEncoder();

	// init capture
	m_p_capture = new CMWCapture();
	m_p_capture->init_capture();
	m_p_capture->refresh_device();
	m_n_capture_index = -1;

	m_p_render_buffer = new CMWRenderBuffer();
	m_p_render_buffer->allocate_buffer(3840 * 2160 * 4);

	sprintf(m_cs_menu_file, "%s", "File");
	sprintf(m_cs_menu_device, "%s", "Devices");

	sprintf(m_cs_menu_video_capture_param, "%s", "Video Capture Param");
	sprintf(m_cs_menu_encode_param, "%s", "Encode Param");
	sprintf(m_cs_menu_start_encode, "%s", "Start Encode");
	sprintf(m_cs_menu_stop_encode, "%s", "Stop Encode");

	m_video_capture_param.m_n_width = 1920;
	m_video_capture_param.m_n_height = 1080;
	m_video_capture_param.m_dw_fourcc = MWFOURCC_YUY2;
	m_video_capture_param.m_ul_frameduration = 166667;

	int t_n_stride = FOURCC_CalcMinStride(m_video_capture_param.m_dw_fourcc,
					      m_video_capture_param.m_n_width,
					      4);
	m_n_data_size = FOURCC_CalcImageSize(m_video_capture_param.m_dw_fourcc,
					     m_video_capture_param.m_n_width,
					     m_video_capture_param.m_n_height,
					     t_n_stride);

	m_n_buffer_size = m_video_capture_param.m_n_width *
			  m_video_capture_param.m_n_height * 4;

	m_f_capture_fps = 0.0;
	m_n_capture_num = 0;
	m_last_capture_clock = 0;
	memset(m_cs_capture_status, 0, 512);
	m_u64_total_capture_num = 0;
	m_capture_status = MW_NOT_CAPTUREING;
	set_capture_status_text(m_cs_capture_status, m_capture_status);

	m_f_encode_fps = 0.0;
	m_n_encode_num = 0;
	m_last_encode_clock = 0;
	memset(m_cs_encode_status, 0, 512);
	m_u64_total_encode_size = 0;
	m_u64_total_encode_num = 0;
	m_encode_status = MW_NOT_ENCODING;
	set_encode_status_text(m_cs_encode_status, m_encode_status);

	memset(m_cs_encoding_adapter, 0, 1024);
	memset(m_wcs_file_name, 0, 1024 * 2);
	memset(m_wcs_file_name_main, 0, 1024 * 2);
	memset(m_wcs_file_name_sub, 0, 1024 * 2);
	memset(m_wcs_file_name_suffixes, 0, 16 * 2);
	m_n_file_name_index = 0;
	m_u64_current_file_size = 0;
	m_p_file = NULL;

	m_p_encode_buffer = new char[4096 * 2160 * 4];

	m_h_event_encode = NULL;
	m_h_event_exit_thread_encode = NULL;
	m_h_thread_encode = NULL;

	m_h_thread_cmd_proc = NULL;
	m_h_event_exit_thread = NULL;
	m_h_event_openfile = NULL;

	m_i32_frame_ignore_cnt = 0;

	InitializeCriticalSection(&m_cs_lock);
}
