#include "mw_hdr_capture_ui.h"
#include "stdio.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

CMWHDRCaptureUI *CMWHDRCaptureUI::s_ui = NULL;
uint32_t CMWHDRCaptureUI::s_u32_ref = 0;
GLFWwindow *CMWHDRCaptureUI::s_p_window = NULL;
HWND CMWHDRCaptureUI::s_h_window = NULL;

static void glfw_error_callback(int t_n_error, const char *t_cs_descip)
{
	printf("Glfw Error %d: %s\n", t_n_error, t_cs_descip);
}

CMWHDRCaptureUI *CMWHDRCaptureUI::create_instance()
{
	if (s_u32_ref == 0) {
		s_ui = new CMWHDRCaptureUI();
		s_u32_ref++;
	}

	return s_ui;
}

CMWHDRCaptureUI *CMWHDRCaptureUI::get_ui()
{
	return s_ui;
}

void CMWHDRCaptureUI::release_ui()
{
	s_u32_ref--;
	if (s_u32_ref <= 0) {
		delete s_ui;
		s_u32_ref = 0;
	}
}

bool CMWHDRCaptureUI::setup_window(char *t_cs_name)
{
	bool t_b_ret = false;
	if (s_p_window != NULL) {
		t_b_ret = true;
		return t_b_ret;
	}

	// s.0 enum encoder
	enum_encoder();

	// s.1 create glfw window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) { //glfw error
		MessageBoxA(NULL, "GLFW ERROR,please confirm your OpenGL",
			    "Warning", MB_OK);
		t_b_ret = false;
		return t_b_ret;
	}

	// s.1.1 set opengl version 3.0
	const char *t_cs_glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	// s.1.2 create window
	s_p_window = glfwCreateWindow(DFT_WINDOW_WIDTH, DFT_WINDOW_HEIGHT,
				      t_cs_name, NULL, NULL);
	if (s_p_window == NULL) {
		MessageBoxA(
			NULL,
			"This program need OpenGL 3.0 support,please confirm your OpenGL Version",
			"Warning", MB_OK);

		t_b_ret = false;
		return t_b_ret;
	}

	// s.1.3 init opengl
	glfwMakeContextCurrent(s_p_window);
	glfwSwapInterval(1); //enable vsync

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
	if (t_n_len >= m_n_len_render)
		t_n_len = m_n_len_render - 1;

	memcpy(m_cs_render, t_p_render_adapter, t_n_len);

	s_h_window = glfwGetWin32Window(s_p_window);

	// s.2 setup imgui

	// s.2.1 setup imgui context
	IMGUI_CHECKVERSION();
	m_p_imgui_ctx = ImGui::CreateContext();

	// s.2.2 setup imgui style
	ImGui::StyleColorsDark();

	// s.2.3 setup platform/render bindings
	ImGui_ImplGlfw_InitForOpenGL(s_p_window, true);
	ImGui_ImplOpenGL3_Init(t_cs_glsl_version);

	// s.3 setup ui
	m_p_ui_manager = new CMWHDRCaptureUIManager();
	m_p_menubar = new CMWHDRCaptureMenuBar(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_menubar);
	m_p_menubar->add_menu(m_cs_menu_file);

	m_p_menubar->add_menu_item(m_cs_menu_file, m_cs_menu_start_record);
	m_p_menubar->set_menu_item_enable(m_cs_menu_file,
					  m_cs_menu_start_record, false);
	m_p_menubar->add_menu_item(m_cs_menu_file, m_cs_menu_stop_record);
	m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_stop_record,
					  false);

	m_p_encoder_adapter = new CMWEncoderAdapter(m_p_ui_manager);
	m_p_menubar->add_menu_submenu(m_cs_menu_file, m_cs_encoder_adapter,
				      m_p_encoder_adapter);
	m_p_menubar->set_menu_sub_menu_enable(m_cs_menu_file,
					      m_cs_encoder_adapter, true);

	m_p_menubar->add_menu(m_cs_menu_device);
	m_p_menubar->add_menu_item(m_cs_menu_device,
				   "None - Only HDMI Supported");
	m_p_menubar->set_menu_item_checked(m_cs_menu_device,
					   "None - Only HDMI Supported", true);
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
					continue;
				} else {
					sprintf(t_cs_name, "%d-%d %s",
						t_channel_info.m_channel_info
							.byBoardIndex,
						t_channel_info.m_channel_info
							.byChannelIndex,
						t_channel_info.m_channel_info
							.szProductName);
				}
				m_p_menubar->add_menu_item(m_cs_menu_device,
							   t_cs_name);
			}
		}
	}

	m_p_menubar->add_menu(m_cs_menu_hdr_setting);
	m_p_menubar->add_menu_item(m_cs_menu_hdr_setting,
				   m_cs_menu_preview_auto);
	m_p_menubar->set_menu_item_checked(m_cs_menu_hdr_setting,
					   m_cs_menu_preview_auto, true);
	m_p_menubar->add_menu_item(m_cs_menu_hdr_setting,
				   m_cs_menu_preview_hdr);
	m_p_menubar->set_menu_item_checked(m_cs_menu_hdr_setting,
					   m_cs_menu_preview_hdr, false);
	m_p_menubar->add_menu_item(m_cs_menu_hdr_setting,
				   m_cs_menu_preview_sdr);
	m_p_menubar->set_menu_item_checked(m_cs_menu_hdr_setting,
					   m_cs_menu_preview_sdr, false);

	m_p_hdr_param = new CMWHDRParam(m_p_ui_manager);
	m_p_hdr_param->set_param_change_callback(callback_hdr_value_change,
						 this);

	m_p_menubar->add_menu_submenu(m_cs_menu_hdr_setting,
				      m_cs_menu_hdr_param, m_p_hdr_param);

	m_p_menubar->set_menu_selected_callback(callback_menu_item_selected,
						this);

	// set for overlay text
	m_p_overlay_capture = new CMWHDROverlatText(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_overlay_capture);
	m_p_overlay_capture->set_name("capture status");

	// s.4 init opengl render
	// init opengl render
	if (m_p_opengl_render == NULL) {
		m_p_opengl_render = new CMWOpenGLRender();
		mw_render_init_t t_init;
		t_init.m_u32_width = m_video_capture_param.m_n_width;
		t_init.m_u32_height = m_video_capture_param.m_n_height;
		t_init.m_mode = get_render_fmt_from_fourcc(
			m_video_capture_param.m_dw_fourcc);
		t_init.m_u8_need_rev = 1;
		m_p_opengl_render->open_hdr(&t_init);
	}

	// s.5 init for file dialog
	m_h_event_openfile = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_stop_record = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_device_changed = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_exit_thread = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_thread_cmd_proc =
		CreateThread(NULL, 0, thread_cmd_process, this, 0, NULL);

	glfwSetWindowRefreshCallback(s_p_window, refresh_cb);

	m_p_encoder_adapter->set_adapters(m_vec_gpu, m_vec_index);

	return t_b_ret;
}

void CMWHDRCaptureUI::show()
{
	cmw_hdr_param_s t_param = m_p_hdr_param->get_ui_video_capture_param();
	m_render_ctrl_ex.m_u8_val_ctrl = t_param.m_i32_luminance;

	while (!glfwWindowShouldClose(s_p_window)) {
		// poll and handle events (inputs,window resize,etc.)
		glfwPollEvents();

		render_ui();
	}
}

void CMWHDRCaptureUI::cleanup_window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(s_p_window);
	glfwTerminate();
}

void CMWHDRCaptureUI::render_ui()
{
	// start the imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	int t_n_width, t_n_height;
	glfwGetWindowSize(s_p_window, &t_n_width, &t_n_height);

	// update overlay text
	memset(m_cs_overlay_text, 0, 1024);
	char t_cs_size[128] = {0};
	int t_n_gb = m_u64_total_encode_size >> 30;
	int t_n_mb = (m_u64_total_encode_size >> 20) & 1023;
	int t_n_kb = (m_u64_total_encode_size >> 10) & 1023;
	int t_n_b = (m_u64_total_encode_size)&1023;
	sprintf(t_cs_size, "%dGB %d MB %dKB %dB", t_n_gb, t_n_mb, t_n_kb,
		t_n_b);

	if (m_h_event_encode != NULL) {

		if (m_file_size.m_u32_mb != t_n_mb) {
			m_file_size.m_u32_gb = t_n_gb;
			m_file_size.m_u32_mb = t_n_mb;
			m_file_size.m_u32_kb = t_n_kb;
			m_file_size.m_u32_b = t_n_b;

			mw_disk_size_t t_disk_size;
			if (get_disk_size(t_disk_size)) {
				if (t_disk_size.m_u32_gb < 1 &&
				    t_disk_size.m_u32_mb < 200) {
					SetEvent(m_h_event_stop_record);
					MessageBoxA(
						s_h_window,
						"There is not enough space on the disk.Encoding will auto exit.",
						"Warning", MB_OK);
				}

				memset(m_cs_disk_size, 0, 256);
				sprintf(m_cs_disk_size, "%dGB %dMB",
					t_disk_size.m_u32_gb,
					t_disk_size.m_u32_mb);
			}
		}
	}

	sprintf(m_cs_overlay_text,
		"render adapter:%s\n"
		"encoder adapter:%s\n"
		"capture status:%s capture fps:%.2f capture frames:%lld\n"
		"encode status:%s encode fps:%.2f encode frames:%lld\n"
		"encode size:%s\n"
		"disk size:%s\n",
		m_cs_render, m_cs_encoding_adapter, m_cs_capture_status,
		m_f_capture_fps, m_u64_total_capture_num, m_cs_encode_status,
		m_f_encode_fps, m_u64_total_encode_num, t_cs_size,
		m_cs_disk_size);
	m_p_overlay_capture->set_text(m_cs_overlay_text);

	m_p_ui_manager->update_ui();

	bool t_b_captureing = m_p_capture->get_is_captureing();
	if (!t_b_captureing) {
		if (!m_p_render_buffer->is_clear())
			m_p_render_buffer->clear_data();
	}

	m_render_ctrl_ex.m_ctrl.m_u32_display_w = t_n_width;
	m_render_ctrl_ex.m_ctrl.m_u32_display_h = t_n_height - 18;

	if (m_hdr_preview_mode == MW_HDR_PREVIEW_AUTO)
		m_render_ctrl_ex.m_u8_hdr_on = m_p_capture->is_hdr();
	else if (m_hdr_preview_mode == MW_HDR_PREVIEW_HDR)
		m_render_ctrl_ex.m_u8_hdr_on = true;
	else
		m_render_ctrl_ex.m_u8_hdr_on = false;

	m_p_opengl_render->render_hdr(&m_render_ctrl_ex, !t_b_captureing);

	//rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(s_p_window, &display_w, &display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(s_p_window);
}

void CMWHDRCaptureUI::refresh_cb(GLFWwindow *t_p_win)
{
	s_ui->render_ui();
}

void CMWHDRCaptureUI::callback_menu_item_selected_proc(char *t_cs_menu,
						       char *t_cs_menu_item,
						       int t_n_index)
{
	if (strcmp(t_cs_menu, m_cs_menu_file) == 0) { // start/stop record
		if (strcmp(t_cs_menu_item, m_cs_menu_start_record) == 0) {
			SetEvent(m_h_event_openfile);
		} else if (strcmp(t_cs_menu_item, m_cs_menu_stop_record) == 0) {
			printf("stop encode clicked\n");
			SetEvent(m_h_event_stop_record);
		}

	} else if (strcmp(t_cs_menu, m_cs_menu_device) == 0) { // device
		m_n_ui_select = t_n_index;
		SetEvent(m_h_event_device_changed);
	} else if (strcmp(t_cs_menu, m_cs_menu_hdr_setting) == 0) {
		if (strcmp(t_cs_menu_item, m_cs_menu_preview_auto) == 0)
			m_hdr_preview_mode = MW_HDR_PREVIEW_AUTO;
		else if (strcmp(t_cs_menu_item, m_cs_menu_preview_hdr) == 0)
			m_hdr_preview_mode = MW_HDR_PREVIEW_HDR;
		else if (strcmp(t_cs_menu_item, m_cs_menu_preview_sdr) == 0)
			m_hdr_preview_mode = MW_HDR_PREVIEW_SDR;
	} else {
		// invalid
	}
}

void CMWHDRCaptureUI::callback_video_capture_proc(BYTE *pbFrame, int cbFrame,
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

	if (m_h_event_encode != NULL) {
		if (TryEnterCriticalSection(&m_cs_lock)) {
			clock_t t_begin = clock();
			m_u64_timestamp = u64TimeStamp;
			memcpy(m_p_encode_buffer, pbFrame, cbFrame);
			clock_t t_end = clock();
			long t_l_duration = t_end - t_begin;
			char t_cs_copy_time[16] = {0};
			sprintf(t_cs_copy_time, "copy:%ld\n", t_l_duration);
			//printf(t_cs_copy_time);
			SetEvent(m_h_event_encode);

			LeaveCriticalSection(&m_cs_lock);
		}
	}
}

void CMWHDRCaptureUI::callback_hdr_value_change_proc(cmw_hdr_param_s t_param)
{
	m_render_ctrl_ex.m_u8_val_ctrl = t_param.m_i32_luminance;
}

DWORD CMWHDRCaptureUI::thread_cmd_process_proc()
{
	DWORD t_dw_re = 0;

	m_u32_frametobe_encode = 0;
	m_n_file_name_index = 0;
	m_clock_start = 0;

	HANDLE t_array_events[4] = {m_h_event_openfile, m_h_event_stop_record,
				    m_h_event_device_changed,
				    m_h_event_exit_thread};
	while (true) {

		DWORD t_dw_ret = WaitForMultipleObjects(4, t_array_events,
							false, INFINITE);
		if (t_dw_ret == WAIT_OBJECT_0) {
			// show file dialog

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

			wsprintf(t_szFileName, L"%d%02d%02d-%02d_%02d_%02d.mp4",
				 t_p_ltime->tm_year + 1900,
				 t_p_ltime->tm_mon + 1, t_p_ltime->tm_mday,
				 t_p_ltime->tm_hour, t_p_ltime->tm_min,
				 t_p_ltime->tm_sec);
			m_ofn.lpstrFilter = _T("MP4 Files (*.mp4)\0*.mp4\0");
			m_ofn.lpstrDefExt = _T("MP4 Files");

			m_ofn.lStructSize = sizeof(m_ofn);
			m_ofn.hwndOwner = s_h_window;

			m_ofn.lpstrFile = t_szFileName;
			m_ofn.nMaxFile = MAX_PATH;
			m_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST |
				      OFN_HIDEREADONLY;

			m_ofn.lpstrInitialDir = t_wcs_path;

			BOOL t_b_re = GetSaveFileName(&m_ofn);
			if (t_b_re) {
				// start encode

				memset(m_wcs_file_name, 0,
				       1024 * sizeof(wchar_t));
				memset(m_wcs_file_name_main, 0,
				       1024 * sizeof(wchar_t));
				memset(m_wcs_file_name_sub, 0,
				       1024 * sizeof(wchar_t));
				memset(m_wcs_file_name_suffixes, 0,
				       1024 * sizeof(wchar_t));
				m_n_file_name_index = 0;

				int t_n_len_file_name =
					wcslen(m_ofn.lpstrFile) *
					sizeof(wchar_t);
				memcpy(m_wcs_file_name, m_ofn.lpstrFile,
				       t_n_len_file_name);
				memcpy(m_wcs_file_name_sub, m_wcs_file_name,
				       t_n_len_file_name);

				wchar_t *t_p_dir_pos = NULL;
				t_p_dir_pos = wcsstr(m_wcs_file_name, L":");
				int t_n_len = 0;
				if (t_p_dir_pos != NULL) {
					t_n_len = t_p_dir_pos - m_wcs_file_name;
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
								s_h_window,
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

				wchar_t *t_p_mp4_pos = NULL;
				t_p_mp4_pos = wcsstr(m_wcs_file_name, L".mp4");
				t_n_len = 0;
				if (t_p_mp4_pos != NULL) {
					t_n_len = t_p_mp4_pos - m_wcs_file_name;
					wsprintf(m_wcs_file_name_suffixes,
						 L".mp4");
				}

				memcpy(m_wcs_file_name_main, m_wcs_file_name,
				       t_n_len * 2);

				m_f_encode_fps = 0.0;
				m_n_encode_num = 0;
				m_last_encode_clock = 0;
				memset(m_cs_encode_status, 0, 512);
				m_u64_total_encode_size = 0;
				m_u64_total_encode_num = 0;

				//m_p_file = mw_mp4_open_ex(m_wcs_file_name);
				m_p_file = NULL;

				int t_n_index =
					m_p_encoder_adapter
						->get_selected_adapter();
				mw_venc_gpu_info_t t_gpu =
					m_vec_gpu.at(t_n_index);

				m_b_need_set_mp4 = true;

				bool t_b_encoding = false;
				if (m_p_encoder != NULL)
					if (t_gpu.platform ==
					    mw_venc_platform_t::
						    MW_VENC_PLATFORM_AMD)
						t_b_encoding = false;
					else
						t_b_encoding =
							m_p_encoder->open_encoder(
								t_gpu.platform,
								&m_venc_param,
								callback_encode,
								this,
								m_vec_index.at(
									t_n_index));

				if (t_b_encoding) {
					m_p_file =
						mw_mp4_open_ex(m_wcs_file_name);

					m_p_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_start_record, false);
					m_p_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_stop_record, true);
					m_p_menubar->set_menu_sub_menu_enable(
						m_cs_menu_file,
						m_cs_encoder_adapter, false);
					m_p_menubar->set_menu_enable(
						m_cs_menu_device, false);

					memset(m_cs_encoding_adapter, 0, 1024);
					sprintf(m_cs_encoding_adapter, "%s",
						t_gpu.gpu_name);
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

					m_clock_start = clock();

					m_file_size.m_u32_gb = 0;
					m_file_size.m_u32_mb = 0;
					m_file_size.m_u32_kb = 0;
					m_file_size.m_u32_b = 0;
				} else {
					memset(m_cs_encoding_adapter, 0, 1024);
					sprintf(m_cs_encoding_adapter, "none");
					m_encode_status = MW_ENCODE_ERROR;
					set_encode_status_text(
						m_cs_encode_status,
						m_encode_status);
					m_p_menubar->set_menu_item_checked(
						m_cs_menu_file,
						m_cs_menu_start_record, false);
					m_p_menubar->set_menu_enable(
						m_cs_menu_device, true);

					// delete file
					if (m_p_file != NULL) {
						mw_mp4_close(m_p_file);
						m_p_file = NULL;
					}

					MessageBoxA(
						s_h_window,
						"This adapter doesn't support P010 encoding",
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
				mw_mp4_close(m_p_file);
				m_p_file = NULL;
			}

			LeaveCriticalSection(&m_cs_lock);

			if (t_b_closed) {
				m_p_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_start_record,
					true);
				m_p_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_stop_record,
					false);
				m_p_menubar->set_menu_sub_menu_enable(
					m_cs_menu_file, m_cs_encoder_adapter,
					true);
				m_p_menubar->set_menu_enable(m_cs_menu_device,
							     true);

				m_encode_status = MW_NOT_ENCODING;
				set_encode_status_text(m_cs_encode_status,
						       m_encode_status);
			} else {
				m_encode_status = MW_ENCODE_STOPERROR;
				set_encode_status_text(m_cs_encode_status,
						       m_encode_status);
				m_p_menubar->set_menu_item_checked(
					m_cs_menu_file, m_cs_menu_start_record,
					false);
				m_p_menubar->set_menu_enable(m_cs_menu_device,
							     false);

				MessageBoxA(s_h_window, "Stop Encoding failed",
					    "Warning", MB_OK);
			}
		} else if (t_dw_ret == WAIT_OBJECT_0 + 2) {
			if (m_n_capture_index != m_n_ui_select) {
				if (m_p_capture->get_is_captureing()) {
					m_p_capture->stop_video_capture();
					m_capture_status = MW_NOT_CAPTUREING;
					set_capture_status_text(
						m_cs_capture_status,
						m_capture_status);
				}
				m_p_capture->close_channel();
			}

			if (m_n_ui_select == 0) {
				m_p_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_start_record,
					false);
				m_p_menubar->set_menu_item_enable(
					m_cs_menu_file, m_cs_menu_stop_record,
					false);
				continue;
			}

			m_u64_total_capture_num = 0;

			bool t_b_open = false;
			t_b_open = m_p_capture->open_channel(m_n_ui_select - 1);
			if (!t_b_open) {
				m_capture_status = MW_CAPTURE_ERROR;
				set_capture_status_text(m_cs_capture_status,
							m_capture_status);
				m_n_capture_index = -1;
				MessageBoxA(s_h_window, "Open Channel failed",
					    "Warning", MB_OK);
				continue;
			}

			m_n_capture_index = m_n_ui_select;

			CMWRenderBuffer *t_p_render_buffer = NULL;
			t_p_render_buffer =
				m_p_opengl_render->get_render_buffer();
			if (m_n_capture_index != -1) {
				bool t_b_capture =
					m_p_capture->start_video_capture(
						m_video_capture_param,
						callback_video_capture, this,
						t_p_render_buffer);
				if (t_b_capture) {
					m_capture_status = MW_CAPTUREING;
					set_capture_status_text(
						m_cs_capture_status,
						m_capture_status);
					if (!m_vec_gpu.empty()) {
						m_p_menubar->set_menu_item_enable(
							m_cs_menu_file,
							m_cs_menu_start_record,
							true);
						m_p_menubar->set_menu_item_enable(
							m_cs_menu_file,
							m_cs_menu_stop_record,
							false);
					}
				} else {
					m_capture_status = MW_CAPTURE_ERROR;
					set_capture_status_text(
						m_cs_capture_status,
						m_capture_status);
					m_p_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_start_record, false);
					m_p_menubar->set_menu_item_enable(
						m_cs_menu_file,
						m_cs_menu_stop_record, false);
					MessageBoxA(s_h_window,
						    "Start Capture failed",
						    "Warning", MB_OK);
				}
			} else {
			}
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

DWORD CMWHDRCaptureUI::thread_encode_proc()
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
					m_p_encoder->put_frame_ex(
						(uint8_t *)m_p_encode_buffer,
						m_u64_timestamp);
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

void CMWHDRCaptureUI::callback_encode_proc(const uint8_t *p_frame,
					   uint32_t frame_len,
					   mw_venc_frame_info_t *p_frame_info)
{
	m_n_encode_num++;
	m_u64_total_encode_num++;

	clock_t t_clock = clock();
	if (m_last_encode_clock == 0)
		m_last_encode_clock = t_clock;

	if (t_clock - m_last_encode_clock >= 1000) {
		m_f_encode_fps = m_n_encode_num * 1000 * 1.0 /
				 (t_clock - m_last_encode_clock);
		m_n_encode_num = 0;
		m_last_encode_clock = t_clock;
	}

	if (m_clock_start > 0) {
		uint64_t t_u64_now = clock();
		if (t_u64_now - m_clock_start >
		    CLOCKS_PER_SEC * 60 * 60) { // split file per hour
			if (m_p_file != NULL) {

				if (p_frame_info->frame_type ==
				    MW_VENC_FRAME_TYPE_IDR) {
					mw_mp4_close(m_p_file);
					m_p_file = NULL;
					m_clock_start = 0;
				}
				if (m_p_file == NULL) {
					wchar_t t_wcs_file_name[1024] = {0};
					m_n_file_name_index++;
					wsprintf(t_wcs_file_name, L"%s-%d.mp4",
						 m_wcs_file_name_main,
						 m_n_file_name_index);
					m_p_file =
						mw_mp4_open_ex(t_wcs_file_name);
					m_b_need_set_mp4 = true;
					m_clock_start = clock();
				}
			}
		}
	}

	if (m_p_file) {
		unsigned char *t_puc_buf = NULL;
		t_puc_buf =
			setup_for_hdr((unsigned char **)&p_frame, &frame_len);

		if (m_b_need_set_mp4) {
			mw_mp4_video_info_t video_track_info;
			memset(&video_track_info, 0, sizeof(video_track_info));
			video_track_info.codec_type = MW_MP4_VIDEO_TYPE_HEVC;
			video_track_info.timescale = 1000;
			video_track_info.width =
				m_video_capture_param.m_n_width;
			video_track_info.height =
				m_video_capture_param.m_n_height;
			video_track_info.hevc.vps = NULL;
			video_track_info.hevc.vps_size = 0;
			video_track_info.hevc.sps = NULL;
			video_track_info.hevc.sps_size = 0;
			video_track_info.hevc.pps = NULL;
			video_track_info.hevc.pps_size = 0;
			mw_mp4_status_t t_status =
				mw_mp4_set_video(m_p_file, &video_track_info);
			m_b_need_set_mp4 = false;
		}

		mw_mp4_write_video(m_p_file, (const unsigned char *)p_frame,
				   frame_len, p_frame_info->pts);
		m_u64_current_file_size += frame_len;
		m_u64_total_encode_size += frame_len;

		free(t_puc_buf);
	}
}

mw_render_fourcc_t
CMWHDRCaptureUI::get_render_fmt_from_fourcc(DWORD t_dw_fourcc)
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

void CMWHDRCaptureUI::enum_encoder()
{
	uint32_t t_u32_platforms = mw_venc_get_support_platfrom();

	mw_venc_handle_t t_h_venc = NULL;
	mw_venc_get_default_param(&m_venc_param);
	m_venc_param.width = 1920;
	m_venc_param.height = 1080;
	m_venc_param.fps.den = 1;
	m_venc_param.fps.num = 60;
	m_venc_param.code_type = MW_VENC_CODE_TYPE_H265;
	m_venc_param.fourcc = MW_VENC_FOURCC_P010;
	m_venc_param.rate_control.mode = MW_VENC_RATECONTROL_CBR;
	m_venc_param.rate_control.target_bitrate = 4096;
	m_venc_param.gop_pic_size = 60;

	uint32_t t_u32_gpus = mw_venc_get_gpu_num();
	for (int i = 0; i < t_u32_gpus; i++) {
		mw_venc_gpu_info_t info;
		mw_venc_get_gpu_info_by_index(i, &info);
		m_vec_gpu.push_back(info);
		m_vec_index.push_back(i);
	}
}

void CMWHDRCaptureUI::set_capture_status_text(char *t_cs_text,
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

void CMWHDRCaptureUI::set_encode_status_text(char *t_cs_text,
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
			"This graphic adapter doesn't support P010.");
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

unsigned char *CMWHDRCaptureUI::setup_for_hdr(unsigned char **t_ppuc_stream_buf,
					      unsigned int *t_pui_stream_len)
{
	unsigned char *pucOutStream = NULL;
	unsigned char *pucSeiStream = NULL;
	unsigned char *pucSpsStream = NULL;
	unsigned int uiSeiLen = 0;
	unsigned int uiSpsLen = 0;
	int iSpsStart, iSpsEnd;
	bool bHaveSps = false;
	unsigned int uiValidFlag = 0;
	HDMI_INFOFRAME_PACKET packet;
	HDMI_HDR_INFOFRAME stHdrInfo = {0};

	ST_HEVC_SPS stSpsInfo = {0};
	bHaveSps = HevcFindSps(*t_ppuc_stream_buf, *t_pui_stream_len,
			       &iSpsStart, &iSpsEnd);
	if (!bHaveSps) {
		return NULL;
	}

	if (!(m_p_capture->is_hdr())) {
		if (m_hdr_preview_mode != MW_HDR_PREVIEW_HDR)
			return NULL;
	}

	if (!(m_p_capture->get_hdr_infoframe_packet(&packet))) {
		if (m_hdr_preview_mode != MW_HDR_PREVIEW_HDR)
			return NULL;
		else {
			packet.hdrInfoFramePayload.display_primaries_lsb_x1 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_x1 =
				195;
			packet.hdrInfoFramePayload.display_primaries_lsb_x2 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_x2 =
				195;
			packet.hdrInfoFramePayload.display_primaries_lsb_x0 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_x0 =
				195;
			packet.hdrInfoFramePayload.display_primaries_lsb_y1 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_y1 =
				195;
			packet.hdrInfoFramePayload.display_primaries_lsb_y2 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_y2 =
				195;
			packet.hdrInfoFramePayload.display_primaries_lsb_y0 =
				80;
			packet.hdrInfoFramePayload.display_primaries_msb_y0 =
				195;
			packet.hdrInfoFramePayload.white_point_lsb_x = 80;
			packet.hdrInfoFramePayload.white_point_msb_x = 195;
			packet.hdrInfoFramePayload.white_point_lsb_y = 80;
			packet.hdrInfoFramePayload.white_point_msb_y = 195;
			packet.hdrInfoFramePayload
				.max_display_mastering_lsb_luminance = 1;
			packet.hdrInfoFramePayload
				.max_display_mastering_msb_luminance = 0;
			packet.hdrInfoFramePayload
				.min_display_mastering_lsb_luminance = 232;
			packet.hdrInfoFramePayload
				.min_display_mastering_msb_luminance = 3;
			packet.hdrInfoFramePayload
				.maximum_content_light_level_lsb = 0;
			packet.hdrInfoFramePayload
				.maximum_content_light_level_msb = 0;
			packet.hdrInfoFramePayload
				.maximum_frame_average_light_level_lsb = 0;
			packet.hdrInfoFramePayload
				.maximum_frame_average_light_level_msb = 0;
		}
	}

	stHdrInfo.usDisplayPromariesX[0] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_x1;
	stHdrInfo.usDisplayPromariesX[0] |=
		packet.hdrInfoFramePayload.display_primaries_msb_x1 << 8;
	stHdrInfo.usDisplayPromariesX[1] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_x2;
	stHdrInfo.usDisplayPromariesX[1] |=
		packet.hdrInfoFramePayload.display_primaries_msb_x2 << 8;
	stHdrInfo.usDisplayPromariesX[2] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_x0;
	stHdrInfo.usDisplayPromariesX[2] |=
		packet.hdrInfoFramePayload.display_primaries_msb_x0 << 8;

	stHdrInfo.usDisplayPromariesY[0] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_y1;
	stHdrInfo.usDisplayPromariesY[0] |=
		packet.hdrInfoFramePayload.display_primaries_msb_y1 << 8;
	stHdrInfo.usDisplayPromariesY[1] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_y2;
	stHdrInfo.usDisplayPromariesY[1] |=
		packet.hdrInfoFramePayload.display_primaries_msb_y2 << 8;
	stHdrInfo.usDisplayPromariesY[2] |=
		packet.hdrInfoFramePayload.display_primaries_lsb_y0;
	stHdrInfo.usDisplayPromariesY[2] |=
		packet.hdrInfoFramePayload.display_primaries_msb_y0 << 8;

	stHdrInfo.usWhitePointX |= packet.hdrInfoFramePayload.white_point_lsb_x;
	stHdrInfo.usWhitePointX |= packet.hdrInfoFramePayload.white_point_msb_x
				   << 8;
	stHdrInfo.usWhitePointY |= packet.hdrInfoFramePayload.white_point_lsb_y;
	stHdrInfo.usWhitePointY |= packet.hdrInfoFramePayload.white_point_msb_y
				   << 8;

	stHdrInfo.uiMaxDisplayMasteringLuminance |=
		packet.hdrInfoFramePayload.max_display_mastering_lsb_luminance;
	stHdrInfo.uiMaxDisplayMasteringLuminance |=
		packet.hdrInfoFramePayload.max_display_mastering_msb_luminance
		<< 8;

	stHdrInfo.uiMinDisplayMasteringLuminance |=
		packet.hdrInfoFramePayload.min_display_mastering_lsb_luminance;
	stHdrInfo.uiMinDisplayMasteringLuminance |=
		packet.hdrInfoFramePayload.min_display_mastering_msb_luminance
		<< 8;

	stHdrInfo.usMaxContentLightLevel |=
		packet.hdrInfoFramePayload.maximum_content_light_level_lsb;
	stHdrInfo.usMaxContentLightLevel |=
		packet.hdrInfoFramePayload.maximum_content_light_level_msb << 8;

	stHdrInfo.usMaxFrameAverageLightLevel |=
		packet.hdrInfoFramePayload.maximum_frame_average_light_level_lsb;
	stHdrInfo.usMaxFrameAverageLightLevel |=
		packet.hdrInfoFramePayload.maximum_frame_average_light_level_msb
		<< 8;

	pucSeiStream = CreateHdrSeiPrefix(&stHdrInfo, &uiSeiLen);

	HevcParseSPS(&stSpsInfo, *t_ppuc_stream_buf + iSpsStart + 5,
		     iSpsEnd - iSpsStart - 5);

	stSpsInfo.short_term_ref_pic_set[0].delta_rps_sign = 0;
	stSpsInfo.short_term_ref_pic_set[0].abs_delta_rps_minus1 = 0;
	stSpsInfo.vui_parameters.max_bytes_per_pic_denom = 2;
	stSpsInfo.vui_parameters.max_bits_per_min_cu_denom = 1;
	stSpsInfo.vui_parameters.log2_max_mv_length_horizontal = 15;
	stSpsInfo.vui_parameters.log2_max_mv_length_vertical = 15;

	pucSpsStream = hevc_add_hdr_to_sps(&stSpsInfo, &uiSpsLen);
	pucOutStream = (unsigned char *)malloc(*t_pui_stream_len -
					       (iSpsEnd - iSpsStart - 5) +
					       uiSpsLen + uiSeiLen);
	if ((pucSeiStream == NULL) || (pucSpsStream == NULL) ||
	    (pucOutStream == NULL)) {
		if (pucSeiStream) {
			free(pucSeiStream);
		}
		if (pucSpsStream) {
			free(pucSpsStream);
		}
		if (pucOutStream) {
			free(pucOutStream);
		}
		printf("malloc fail\n");
		return NULL;
	}
	memcpy(pucOutStream, pucSeiStream, uiSeiLen);
	memcpy(pucOutStream + uiSeiLen, *t_ppuc_stream_buf, iSpsStart + 5);
	memcpy(pucOutStream + uiSeiLen + iSpsStart + 5, pucSpsStream, uiSpsLen);
	memcpy(pucOutStream + uiSeiLen + iSpsStart + 5 + uiSpsLen,
	       *t_ppuc_stream_buf + iSpsEnd, *t_pui_stream_len - iSpsEnd);
	*t_pui_stream_len = *t_pui_stream_len - iSpsEnd + uiSpsLen + iSpsStart +
			    5 + uiSeiLen;
	*t_ppuc_stream_buf = pucOutStream;
	free(pucSeiStream);
	free(pucSpsStream);

	return pucOutStream;
}

bool CMWHDRCaptureUI::get_disk_size(mw_disk_size_t &t_size)
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

CMWHDRCaptureUI::CMWHDRCaptureUI()
{
	m_p_imgui_ctx = NULL;
	m_p_opengl_render = NULL;

	sprintf(m_cs_menu_file, "File");
	sprintf(m_cs_menu_start_record, "Start Record");
	sprintf(m_cs_menu_stop_record, "Stop Record");
	sprintf(m_cs_encoder_adapter, "Encode Adapter");

	m_f_capture_fps = 0;
	m_u64_current_file_size = 0;
	m_u64_total_capture_num = 0;
	m_u64_total_encode_num = 0;
	m_u64_total_encode_size = 0;
	m_f_encode_fps = 0;

	m_p_file = NULL;

	memset(m_wcs_file_name_sub, 0, 1024 * 2);
	wsprintf(m_wcs_file_name_sub, L"none");

	memset(m_cs_encoding_adapter, 0, 1024);
	sprintf(m_cs_encoding_adapter, "none");

	memset(m_wcs_file_name_sub, 0, 1024 * 2);
	wsprintf(m_wcs_file_name_sub, L"none");

	memset(m_cs_disk_size, 0, 256);
	sprintf(m_cs_disk_size, "not selected");

	m_capture_status = MW_NOT_CAPTUREING;
	memset(m_cs_capture_status, 0, 512);
	set_capture_status_text(m_cs_capture_status, m_capture_status);
	m_encode_status = MW_NOT_ENCODING;
	memset(m_cs_encode_status, 0, 512);
	set_encode_status_text(m_cs_encode_status, m_encode_status);

	memset(m_wcs_dir, 0, sizeof(WCHAR) * 256);
	wsprintf(m_wcs_dir, L"not selected");

	m_n_len_render = 256;
	memset(m_cs_render, 0, m_n_len_render);

	m_p_encode_buffer = new char[4096 * 2160 * 4];

	m_h_event_encode = NULL;
	m_h_event_stop_record = NULL;
	m_h_event_exit_thread_encode = NULL;
	m_h_thread_encode = NULL;

	m_h_thread_cmd_proc = NULL;
	m_h_event_exit_thread = NULL;
	m_h_event_openfile = NULL;

	sprintf(m_cs_menu_device, "Devices");

	sprintf(m_cs_menu_hdr_setting, "HDR Settings");
	sprintf(m_cs_menu_preview_hdr, "Preview HDR");
	sprintf(m_cs_menu_preview_sdr, "Preview SDR");
	sprintf(m_cs_menu_preview_auto, "Preview Auto");
	sprintf(m_cs_menu_hdr_param, "HDR Param");

	m_video_capture_param.m_n_width = 1920;
	m_video_capture_param.m_n_height = 1080;
	m_video_capture_param.m_dw_fourcc = MWFOURCC_P010;
	m_video_capture_param.m_ul_frameduration = 166667;

	m_hdr_preview_mode = MW_HDR_PREVIEW_AUTO;

	// init capture
	m_p_capture = new CMWCapture();
	m_p_capture->init_capture();
	m_p_capture->refresh_device();
	m_n_capture_index = -1;
	m_n_ui_select = -1;

	m_p_render_buffer = new CMWRenderBuffer();
	m_p_render_buffer->allocate_buffer(3840 * 2160 * 4);

	// init encode part
	mw_venc_init();

	// create encoder
	m_p_encoder = new CMWHDREncoder();

	InitializeCriticalSection(&m_cs_lock);
}

CMWHDRCaptureUI::~CMWHDRCaptureUI()
{
	if (m_p_capture != NULL) {
		m_p_capture->stop_video_capture();
		m_p_capture->deinit_capture();

		delete m_p_capture;
		m_p_capture = NULL;
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

	EnterCriticalSection(&m_cs_lock);

	m_p_encoder->set_is_encoding(false);
	bool t_b_closed = false;
	if (m_p_encoder != NULL)
		t_b_closed = m_p_encoder->close_encoder();

	if (m_p_file != NULL) {
		mw_mp4_close(m_p_file);
		m_p_file = NULL;
	}

	LeaveCriticalSection(&m_cs_lock);

	if (m_h_thread_cmd_proc) {
		SetEvent(m_h_event_exit_thread);
		WaitForSingleObject(m_h_thread_cmd_proc, INFINITE);
		CloseHandle(m_h_thread_cmd_proc);
		m_h_thread_cmd_proc = NULL;
		CloseHandle(m_h_event_exit_thread);
		m_h_event_exit_thread = NULL;
	}

	if (m_h_event_openfile) {
		CloseHandle(m_h_event_openfile);
		m_h_event_openfile = NULL;
	}

	if (m_h_event_stop_record) {
		CloseHandle(m_h_event_stop_record);
		m_h_event_stop_record = NULL;
	}

	if (m_h_event_device_changed) {
		CloseHandle(m_h_event_device_changed);
		m_h_event_device_changed = NULL;
	}

	if (m_p_render_buffer != NULL) {
		delete m_p_render_buffer;
		m_p_render_buffer = NULL;
	}

	if (m_p_encoder) {
		delete m_p_encoder;
		m_p_encoder = NULL;
	}

	mw_venc_deinit();

	if (m_p_encode_buffer != NULL) {
		delete m_p_encode_buffer;
		m_p_encode_buffer = NULL;
	}

	DeleteCriticalSection(&m_cs_lock);
}
