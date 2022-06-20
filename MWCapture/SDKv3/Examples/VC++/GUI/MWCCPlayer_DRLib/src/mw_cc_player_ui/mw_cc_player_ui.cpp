/************************************************************************************************/
// mw_cc_player_ui.cpp : implementation of the CMWCCPlayerUI class

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

#include "mw_cc_player_ui.h"
#include "stdio.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

CMWCCPlayerUI*	CMWCCPlayerUI::s_ui			= NULL;
uint32_t		CMWCCPlayerUI::s_u32_ref	= 0;
GLFWwindow*		CMWCCPlayerUI::s_p_window	= NULL;
HWND			CMWCCPlayerUI::s_h_window	= NULL;

static void glfw_error_callback(int t_n_error,const char* t_cs_descp)
{
	printf("Error GLFW:%d - %s\n",t_n_error,t_cs_descp);
}

CMWCCPlayerUI* CMWCCPlayerUI::create_instance()
{
	if(0 == s_u32_ref){
		s_ui = new CMWCCPlayerUI();
		s_u32_ref++;
	}

	return s_ui;
}

CMWCCPlayerUI* CMWCCPlayerUI::get_ui()
{
	return s_ui;
}

void CMWCCPlayerUI::release_ui()
{
	s_u32_ref--;
	if(s_u32_ref<=0){
		delete s_ui;
		s_u32_ref = 0;
	}
}

bool CMWCCPlayerUI::setup_window(char* t_cs_name)
{
	bool t_b_ret = false;

	if(NULL != s_p_window){
		t_b_ret = true;
		return t_b_ret;
	}

	// s.0 enum encoder
	enum_encoder();

	// s.1 create glfw window
	glfwSetErrorCallback(glfw_error_callback);
	if(!glfwInit()){
		MessageBoxA(NULL,
			"GLFW ERROR,please confirm your OpenGL",
			"Warning",
			MB_OK);
		t_b_ret = false;
		return t_b_ret;
	}

	// s.1.1 set opengl version 3.0
	const char* t_cs_glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,0);

	// s.1.2 create window
	s_p_window = glfwCreateWindow(
		DFT_WINDOW_WIDTH,
		DFT_WINDOW_HEIGHT,
		t_cs_name,
		NULL,
		NULL
		);
	if(NULL == s_p_window){
		MessageBoxA(NULL,
			"This program need OpenGL 3.0 support,please confirm your OpenGL Version",
			"Warning",
			MB_OK);
		t_b_ret = false;
		return t_b_ret;
	}

	// s.1.3 init opengl
	glfwMakeContextCurrent(s_p_window);
	glfwSwapInterval(1);	// enable v-sync

	t_b_ret = gl3wInit()!=0;
	if(t_b_ret){
		MessageBoxA(NULL,
			"GL3W ERROR,please confirm your OpenGL",
			"Warning",
			MB_OK);
		return t_b_ret;
	}

	const GLubyte* t_p_render_adapter = NULL;
	t_p_render_adapter = glGetString(GL_RENDERER);
	if(t_p_render_adapter==NULL){
		MessageBoxA(NULL,
			"Failed to get Adapter",
			"Warning",
			MB_OK);
		return false;
	}
	int t_n_len = strlen((const char*)t_p_render_adapter);
	if(t_n_len>=m_n_len_render)
		t_n_len = m_n_len_render - 1;

	memcpy(m_cs_render,t_p_render_adapter,t_n_len);


	s_h_window = glfwGetWin32Window(s_p_window);


	// s.2 setup imgui

	// s.2.1 setup imgui context
	IMGUI_CHECKVERSION();
	m_p_imgui_ctx = ImGui::CreateContext();

	// s.2.2 setup imgui style
	ImGui::StyleColorsDark();

	// s.2.3 setup platform/render bindings
	ImGui_ImplGlfw_InitForOpenGL(s_p_window,true);
	ImGui_ImplOpenGL3_Init(t_cs_glsl_version);

	// s.3 setup ui
	m_p_ui_manager = new CMWCCPlayerUIManager();
	m_p_menubar = new CMCCPlayerMenuBar(m_p_ui_manager);

	// set for cc layer
	m_p_cc_layer = new CMWCCLayer(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_cc_layer);
	m_p_cc_layer->set_name("cc layer");

	m_p_ui_manager->add_widget(m_p_menubar);
	m_p_menubar->add_menu(m_cs_menu_file);

	m_p_menubar->add_menu_item(m_cs_menu_file,m_cs_menu_start_record);
	m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_start_record,false);
	m_p_menubar->add_menu_item(m_cs_menu_file,m_cs_menu_stop_record);
	m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_stop_record,false);

	m_p_encoder_adapter = new CMWEncoderAdapter(m_p_ui_manager);
	m_p_menubar->add_menu_submenu(m_cs_menu_file,m_cs_encoder_adapter,m_p_encoder_adapter);
	m_p_menubar->set_menu_sub_menu_enable(m_cs_menu_file,m_cs_encoder_adapter,true);

	m_p_menubar->add_menu(m_cs_menu_device);
	m_p_menubar->add_menu_item(m_cs_menu_device,"None - Only SDI Supported");
	m_p_menubar->set_menu_item_checked(m_cs_menu_device,"None - Only SDI Supported",true);
	if(NULL == m_p_capture)
		m_p_capture = new CMWCapture();

	if(m_p_capture){
		m_p_capture->refresh_device();
		int t_n_channel_count = m_p_capture->get_channel_count();
		for (int i = 0; i < t_n_channel_count; i++) {
			cmw_channel_info_s t_channel_info;
			bool t_b_info = m_p_capture->get_channel_info(i,&t_channel_info);
			if (t_b_info) {
				char t_cs_name[256] = {0};
				if (t_channel_info.m_channel_info.wFamilyID == MW_FAMILY_ID_USB_CAPTURE) {
					continue;
				}
				else {
					sprintf(t_cs_name, "%d-%d %s", t_channel_info.m_channel_info.byBoardIndex,
						t_channel_info.m_channel_info.byChannelIndex,
						t_channel_info.m_channel_info.szProductName);
				}
				m_p_menubar->add_menu_item(m_cs_menu_device, t_cs_name);
			}
		}
	}

	m_p_cc_setting = new CMWCCSetting(m_p_ui_manager);
	m_p_menubar->add_menu(m_cs_menu_cc_settings);
	m_p_menubar->add_menu_submenu(m_cs_menu_cc_settings,m_cs_menu_cc_settings,m_p_cc_setting);

	m_p_menubar->set_menu_selected_callback(callback_menu_item_selected, this);

	

	// set for overlay text
	m_p_overlay_capture = new CMWCCOverlayText(m_p_ui_manager);
	m_p_ui_manager->add_widget(m_p_overlay_capture);
	m_p_overlay_capture->set_name("capture status");

	// s.4 init opengl render
	if(NULL == m_p_opengl_render){
		m_p_opengl_render = new CMWOpenGLRender();
		mw_render_init_t t_init;
		t_init.m_u32_width = m_video_capture_param.m_n_width;
		t_init.m_u32_height = m_video_capture_param.m_n_height;
		t_init.m_mode = get_render_fmt_from_fourcc(m_video_capture_param.m_dw_fourcc);
		t_init.m_u8_need_rev = 1;
		m_p_opengl_render->open_sdr(&t_init);
	}

	// s.5 init for file dialog
	m_h_event_openfile = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_h_event_stop_record = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_h_event_device_changed = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_h_event_exit_thread = CreateEvent(NULL,FALSE,FALSE,NULL);
	m_h_thread_cmd_proc = CreateThread(NULL,0,thread_cmd_process,this,0,NULL);


	glfwSetWindowRefreshCallback(s_p_window,refresh_cb);

	m_p_encoder_adapter->set_adapters(m_vec_gpu,m_vec_index);

	// set cc
	glGenTextures(1,&m_glsl_cc);
	m_p_cc_screen = new unsigned char[1920*1080*4];
	memset(m_p_cc_screen,0,1920*1080*4);

	m_p_cc_layer->set_texture_id(m_glsl_cc);

	m_cc_param = m_p_cc_setting->get_cc_param();

	m_n_cc_width = m_cc_param.m_n_cc_width;
	m_n_cc_height = m_cc_param.m_n_cc_height;
	m_n_cc_size = m_n_cc_width*m_n_cc_height*4;
	m_n_target_x = 0;
	m_n_target_y = 0;
	m_n_target_width = m_n_cc_width;
	m_n_target_height = m_n_cc_height;

	m_mcf_font.bitalic = false;
	m_mcf_font.bunderline = false;
	m_mcf_font.nft_error = 0;
	m_mcf_font.nfont_ratio = m_cc_param.m_n_font_size;
	memset(&(m_mcf_font.mccbacl_color), 0, sizeof(mw_cc_color_t));
	memset(&(m_mcf_font.mccfont_color), 0, sizeof(mw_cc_color_t));
	m_mcf_font.mcccustom_back_color.r = m_cc_param.m_back_color[0]*255;
	m_mcf_font.mcccustom_back_color.g = m_cc_param.m_back_color[1]*255;
	m_mcf_font.mcccustom_back_color.b = m_cc_param.m_back_color[2]*255;
	m_mcf_font.mcccustom_back_color.a = m_cc_param.m_back_color[3]*255;

	m_mcf_font.mcccustom_font_color.r = m_cc_param.m_fore_color[0]*255;
	m_mcf_font.mcccustom_font_color.g = m_cc_param.m_fore_color[0]*255;
	m_mcf_font.mcccustom_font_color.b = m_cc_param.m_fore_color[0]*255;
	m_mcf_font.mcccustom_font_color.a = m_cc_param.m_fore_color[0]*255;
	m_mcf_font.nfont_height = m_n_cc_height / 15;
	m_mcf_font.nfont_width = m_n_cc_width / 32;
	m_mcf_font.pft_face = NULL;
	m_mcf_font.pft_lib = NULL;
	m_mcf_font.bdefault = true;
	m_mcf_font.bcustom_back_color = 1;
	m_mcf_font.bcustom_font_color = 1;
	memset(m_mcf_font.csfont_path, 0, MW_FONT_PATH_MAX_LEN);

	m_p_file = NULL;

	return t_b_ret;
}

void CMWCCPlayerUI::show()
{
	while(!glfwWindowShouldClose(s_p_window)){
		// poll and handle events (inputs,window resize,etc.)
		glfwPollEvents();

		render_ui();
	}
}

void CMWCCPlayerUI::cleanup_window()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(s_p_window);
	glfwTerminate();
}

void CMWCCPlayerUI::render_ui()
{
	// start the imgui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	int t_n_width,t_n_height;
	glfwGetWindowSize(s_p_window,&t_n_width,&t_n_height);

	// update overlay text
	memset(m_cs_overlay_text,0,1024);
	char t_cs_size[128]={0};
	int t_n_gb = m_u64_total_encode_size>>30;
	int t_n_mb = (m_u64_total_encode_size>>20)&1023;
	int t_n_kb = (m_u64_total_encode_size>>10)&1023;
	int t_n_b = (m_u64_total_encode_size)&1023;
	sprintf(t_cs_size,
		"%dGB %dMB %dKB %dB",
		t_n_gb,
		t_n_mb,
		t_n_kb,
		t_n_b);

	if(m_h_event_encode!=NULL){
		if(m_file_size.m_u32_mb!=t_n_mb){
			m_file_size.m_u32_gb = t_n_gb;
			m_file_size.m_u32_mb = t_n_mb;
			m_file_size.m_u32_kb = t_n_kb;
			m_file_size.m_u32_b = t_n_b;

			mw_disk_size_t t_disk_size;
			if(get_disk_size(t_disk_size)){
				if(t_disk_size.m_u32_gb<1&&
				   t_disk_size.m_u32_mb<200){
					   SetEvent(m_h_event_stop_record);
					   MessageBoxA(
						   s_h_window,
						   "There is not enough space on the disk.Encoding will auto exit.",
						   "Warning",
						   MB_OK);
				}

				memset(m_cs_disk_size,0,256);
				sprintf(m_cs_disk_size,
					"%dGB %dMB",
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
		m_cs_render,
		m_cs_encoding_adapter,
		m_cs_capture_status,m_f_capture_fps,m_u64_total_capture_num,
		m_cs_encode_status,m_f_encode_fps,m_u64_total_encode_num,
		t_cs_size,
		m_cs_disk_size);
	m_p_overlay_capture->set_text(m_cs_overlay_text);

	EnterCriticalSection(&m_cs_lock_cc);

	glBindTexture(GL_TEXTURE_2D, m_glsl_cc);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_n_cc_width, m_n_cc_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_p_cc_screen);

	if(m_p_cc_setting->update_cc_param(&m_cc_param)){
		if(m_cc_param.m_b_cc708!=m_b_show_cc708||
			m_cc_param.m_b_cc608!=m_b_show_cc608){
				m_b_show_cc608 = m_cc_param.m_b_cc608;
				m_b_show_cc708 = m_cc_param.m_b_cc708;
				MWSetCC708DecodeType(m_p_cc708_decoder,m_b_show_cc608,m_b_show_cc708);
		}

		MW_CC_RESULT t_cc_res = MW_CC_NO_ERROR;
		if(m_cc_param.m_n_cc608_select!=m_n_cc608_out){
			m_n_cc608_out = m_cc_param.m_n_cc608_select;
			if(m_n_cc608_out==0){
				t_cc_res = MWEnableOutputChannel(m_p_cc708_decoder,MW_CC608_ALL);
			}
			else{
				int t_n_ch = 0-m_n_cc608_out;
				t_cc_res = MWDisableOutputChannel(m_p_cc708_decoder,MW_CC608_ALL);
				t_cc_res = MWEnableOutputChannel(m_p_cc708_decoder,(mw_cc708_channel_t)t_n_ch);
			}
		}


		//m_n_cc_width = m_cc_param.m_n_cc_width;
		//m_n_cc_height = m_cc_param.m_n_cc_height;
		//m_n_cc_size = m_n_cc_width*m_n_cc_height*4;
		m_n_target_x = 0;
		m_n_target_y = 0;
		//m_n_target_width = m_n_cc_width;
		//m_n_target_height = m_n_cc_height;

		m_mcf_font.nfont_ratio = m_cc_param.m_n_font_size;
		memset(&(m_mcf_font.mccbacl_color), 0, sizeof(mw_cc_color_t));
		memset(&(m_mcf_font.mccfont_color), 0, sizeof(mw_cc_color_t));
		m_mcf_font.mcccustom_back_color.r = m_cc_param.m_back_color[0]*255;
		m_mcf_font.mcccustom_back_color.g = m_cc_param.m_back_color[1]*255;
		m_mcf_font.mcccustom_back_color.b = m_cc_param.m_back_color[2]*255;
		m_mcf_font.mcccustom_back_color.a = m_cc_param.m_back_color[3]*255;

		m_mcf_font.mcccustom_font_color.r = m_cc_param.m_fore_color[0]*255;
		m_mcf_font.mcccustom_font_color.g = m_cc_param.m_fore_color[1]*255;
		m_mcf_font.mcccustom_font_color.b = m_cc_param.m_fore_color[2]*255;
		m_mcf_font.mcccustom_font_color.a = m_cc_param.m_fore_color[3]*255;
		//m_mcf_font.nfont_height = m_n_cc_height / 15;
		//m_mcf_font.nfont_width = m_n_cc_width / 32;
	}

	m_p_ui_manager->update_ui();

	LeaveCriticalSection(&m_cs_lock_cc);

	bool t_b_capturing = m_p_capture->get_is_captureing();
	if(!t_b_capturing){
		if(!m_p_render_buffer->is_clear())
			m_p_render_buffer->clear_data();
	}

	m_render_ctrl.m_u32_display_w = t_n_width;
	m_render_ctrl.m_u32_display_h = t_n_height - 18;

	m_p_opengl_render->render_sdr(&m_render_ctrl,!t_b_capturing);

	// rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(s_p_window);
}

void CMWCCPlayerUI::refresh_cb(GLFWwindow* t_p_win)
{
	s_ui->render_ui();
}

void 
CMWCCPlayerUI::callback_menu_item_selected_proc(
	char* t_cs_menu, 
	char* t_cs_menu_item, 
	int t_n_index)
{
	if(strcmp(t_cs_menu,m_cs_menu_file)==0){		// start/stop record
		if(strcmp(t_cs_menu_item,m_cs_menu_start_record)==0){
			SetEvent(m_h_event_openfile);
		}
		else if(strcmp(t_cs_menu_item,m_cs_menu_stop_record)==0){
			printf("stop encode clicked\n");
			SetEvent(m_h_event_stop_record);
		}

	}else if(strcmp(t_cs_menu,m_cs_menu_device)==0){// device
		m_n_ui_select = t_n_index;
		SetEvent(m_h_event_device_changed);
	}else{
		// invalid 
	}
}

void 
CMWCCPlayerUI::callback_video_capture_proc(
	BYTE* pbFrame, 
	int cbFrame, 
	UINT64 u64TimeStamp)
{
	// calac real fps
	m_n_capture_num++;
	m_u64_total_capture_num++;
	clock_t t_clock = clock();
	if (m_last_capture_clock == 0)
		m_last_capture_clock = t_clock;
	if (t_clock - m_last_capture_clock >= 1000) {
		m_f_capture_fps = m_n_capture_num*1000*1.0/ (t_clock-m_last_capture_clock);
		m_n_capture_num = 0;
		m_last_capture_clock = t_clock;
	}

	if(NULL != m_h_event_encode){
		if(TryEnterCriticalSection(&m_cs_lock)){
			m_u64_timestamp = u64TimeStamp;
			memcpy(m_p_encode_buffer,pbFrame,cbFrame);
			SetEvent(m_h_event_encode);
			LeaveCriticalSection(&m_cs_lock);
		}
	}
}

void 
CMWCCPlayerUI::callback_anc_decode_proc(int service)
{
	MW_CC_RESULT t_hr = MW_CC_NO_ERROR;


	int t_n_cc_width = 0;
	int t_n_cc_height = 0;
	int t_n_cc_size = 0;
	int t_n_target_width = 0;
	int t_n_target_height = 0;

	EnterCriticalSection(&m_cs_lock_cc);
	if(m_n_cc_width!= m_cc_param.m_n_cc_width||
	   m_n_cc_height!= m_cc_param.m_n_cc_height){
		   t_n_cc_width = m_cc_param.m_n_cc_width;
		   t_n_cc_height = m_cc_param.m_n_cc_height;
		   t_n_cc_size = t_n_cc_width*t_n_cc_height*4;
		   t_n_target_width = t_n_cc_width;
		   t_n_target_height = t_n_cc_height;
		   m_mcf_font.nfont_height = t_n_cc_height / 15;
		   m_mcf_font.nfont_width = t_n_cc_width / 32;
	}
	else{
		t_n_cc_width = m_n_cc_width;
		t_n_cc_height = m_n_cc_height;
		t_n_cc_size = m_n_cc_size;
		t_n_target_width = m_n_target_width;
		t_n_target_height = m_n_target_height;
	}
	
	if(service<0){
		mw_cc608_buffer_t *t_p_608_buffer=&(m_p_cc708_decoder->cc608_buffer);
		if(m_p_cc_screen){
			t_hr = MWRenderCC608BufferArea(
				m_p_cc_screen,
				t_n_cc_width,
				t_n_cc_height,
				t_n_cc_size,
				m_n_target_x,
				m_n_target_y,
				t_n_target_width,
				t_n_target_height,
				t_p_608_buffer,
				&m_mcf_font,
				m_p_cc_render);
		}
	}
	else if(service>=0 && service<=62){
		mw_cc708_tv_window_screen_t* t_p_window_screen = &(m_p_cc708_decoder->cc708_window_screen);
		if(m_p_cc_screen){
			t_hr = MWRenderCC708BufferArea(
				m_p_cc_screen,
				t_n_cc_width,
				t_n_cc_height,
				t_n_cc_size,
				m_n_target_x,
				m_n_target_y,
				t_n_target_width,
				t_n_target_height,
				t_p_window_screen,
				&m_mcf_font,
				m_p_cc_render);
			if(MW_CC_NO_ERROR == t_hr){
				//fill render buffer
			}
		}
	}
	
	if(MW_CC_NO_ERROR == t_hr){
		m_n_cc_width = m_cc_param.m_n_cc_width;
		m_n_cc_height = m_cc_param.m_n_cc_height;
		m_n_cc_size = m_n_cc_width*m_n_cc_height*4;
		m_n_target_width = m_n_cc_width;
		m_n_target_height = m_n_cc_height;
		m_mcf_font.nfont_height = m_n_cc_height / 15;
		m_mcf_font.nfont_width = m_n_cc_width / 32;
	}

	LeaveCriticalSection(&m_cs_lock_cc);
}

void 
CMWCCPlayerUI::callback_anc_capture_proc(
	MWCAP_SDI_ANC_PACKET t_anc_packet,
	UINT64 t_u64_timestamp)
{
	if(t_anc_packet.byDID!=0x61)
		return;

	if(t_anc_packet.bySDID!=0x01)
		return;

	if(t_anc_packet.byDC < 3)
		return;

	int t_n_size = t_anc_packet.byDC + 3;
	void* t_p_anc = (void*)&t_anc_packet;
	MWDecodeCC708(m_p_cc708_decoder,(unsigned char*)t_p_anc,t_n_size);

	//record
	EnterCriticalSection(&m_cs_lock_file);
	if(m_p_file!=NULL){
		if(m_b_set_anc_track){
			mw_mp4_subtitle_info_t subtitle_track_info;
			subtitle_track_info.codec_type = MW_MP4_SUBTITLE_TYPE_CC708;
			subtitle_track_info.timescale = 1000;

			mw_mp4_status_t t_status = mw_mp4_set_subtitle(m_p_file, &subtitle_track_info);
			m_b_set_anc_track = false;
		}

		if(!m_b_set_anc_track){
			mw_mp4_write_subtitle(
				m_p_file, 
				(const unsigned char*)t_p_anc, 
				t_n_size, 
				t_u64_timestamp/10000);
		}

	}
	LeaveCriticalSection(&m_cs_lock_file);
}

DWORD CMWCCPlayerUI::thread_cmd_process_proc()
{
	DWORD t_dw_re = 0;

	m_n_file_name_index = 0;
	m_clock_start = 0;

	HANDLE t_array_events[4] = {
		m_h_event_openfile,
		m_h_event_stop_record,
		m_h_event_device_changed,
		m_h_event_exit_thread
	};
	while(true){
		DWORD t_dw_ret = WaitForMultipleObjects(4,t_array_events,false,INFINITE);
		if(WAIT_OBJECT_0 == t_dw_ret){
			// show file dialog
			tm *t_p_ltime = NULL;
			time_t t_tt = time(NULL);
			t_p_ltime = localtime(&t_tt);

			char t_cs_path[1024]={0};
			strcpy_s(t_cs_path,getenv("HOMEDRIVE"));
			strcat_s(t_cs_path,getenv("HOMEPATH"));
			strcat_s(t_cs_path,"\\Magewell");
			WCHAR t_wcs_path[1024]={0};
			int nLen = MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, t_cs_path, -1, NULL, 0 );
			MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, t_cs_path, -1, t_wcs_path, nLen );

			WCHAR t_szFileName[MAX_PATH] = {0};
			ZeroMemory(&m_ofn,sizeof(m_ofn));

			wsprintf(t_szFileName, L"%d%02d%02d-%02d_%0d_%02d.mp4",
				t_p_ltime->tm_year+1900,
				t_p_ltime->tm_mon+1,
				t_p_ltime->tm_mday,
				t_p_ltime->tm_hour,
				t_p_ltime->tm_min,
				t_p_ltime->tm_sec);
			m_ofn.lpstrFilter = _T("MP4 Files (*.mp4)\0*.mp4\0");
			m_ofn.lpstrDefExt = _T("MP4 Files");

			m_ofn.lStructSize = sizeof(m_ofn);
			m_ofn.hwndOwner = s_h_window;

			m_ofn.lpstrFile = t_szFileName;
			m_ofn.nMaxFile = MAX_PATH;
			m_ofn.Flags = OFN_EXPLORER|OFN_FILEMUSTEXIST|OFN_HIDEREADONLY;

			m_ofn.lpstrInitialDir = t_wcs_path;

			BOOL t_b_re = GetSaveFileName(&m_ofn);
			if(t_b_re){
				// start encode

				memset(m_wcs_file_name,0,1024*2);
				memset(m_wcs_file_name_main,0,1024*2);
				memset(m_wcs_file_name_sub,0,1024*2);
				memset(m_wcs_file_name_suffixes,0,1024*2);
				m_n_file_name_index = 0;

				wsprintf(m_wcs_file_name,L"%s",m_ofn.lpstrFile);

				int t_n_len_file_name = wcslen(m_wcs_file_name);
				memcpy(m_wcs_file_name_sub,m_wcs_file_name,t_n_len_file_name*2);

				wchar_t* t_p_dir_pos = NULL;
				t_p_dir_pos = wcsstr(m_wcs_file_name,L":");
				int t_n_len = 0;
				if(t_p_dir_pos!=NULL){
					t_n_len = t_p_dir_pos - m_wcs_file_name;
					memset(m_wcs_dir,0,256);
					memcpy(m_wcs_dir,m_wcs_file_name,(t_n_len+1)*2);

					// check disk size
					mw_disk_size_t t_disk_size;
					if(get_disk_size(t_disk_size)){
						if(t_disk_size.m_u32_gb<1&&
						   t_disk_size.m_u32_mb<200){
							   MessageBoxA(
								   s_h_window,
								   "There is not enough space on the disk.",
								   "Warning",
								   MB_OK);
							   memset(m_cs_disk_size,0,256);
							   sprintf(m_cs_disk_size,
								   "not selected");
							   continue;
						}
					}
					memset(m_cs_disk_size,0,256);
					sprintf(m_cs_disk_size,
						"%dGB %dMB",
						t_disk_size.m_u32_gb,
						t_disk_size.m_u32_mb);
				}

				wchar_t* t_p_mp4_pos = NULL;
				t_p_mp4_pos = wcsstr(m_wcs_file_name,L".mp4");
				t_n_len = 0;
				if(t_p_mp4_pos!=NULL){
					t_n_len = t_p_mp4_pos - m_wcs_file_name;
					wsprintf(m_wcs_file_name_suffixes,L".mp4");
				}

				memcpy(m_wcs_file_name_main,m_wcs_file_name,t_n_len*2);

				m_f_encode_fps = 0.0;
				m_n_encode_num = 0;
				m_last_encode_clock = 0;
				memset(m_cs_encode_status,0,512);
				m_u64_total_encode_size = 0;
				m_u64_total_encode_num = 0;

				//m_p_file = mw_mp4_open_ex(m_wcs_file_name);
				m_p_file = NULL;

				int t_n_index = m_p_encoder_adapter->get_selected_adapter();
				mw_venc_gpu_info_t t_gpu = m_vec_gpu.at(t_n_index);

				m_b_need_set_mp4 = true;
				m_b_set_anc_track = true;

				bool t_b_encoding = false;
				if (m_p_encoder != NULL) 
					t_b_encoding = m_p_encoder->open_encoder(t_gpu.platform, &m_venc_param, callback_encode, this,m_vec_index.at(t_n_index));

				if (t_b_encoding) {
					m_p_file = mw_mp4_open_ex(m_wcs_file_name);

					m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_start_record, false);
					m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_stop_record, true);
					m_p_menubar->set_menu_sub_menu_enable(m_cs_menu_file,m_cs_encoder_adapter,false);
					m_p_menubar->set_menu_enable(m_cs_menu_device,false);

					memset(m_cs_encoding_adapter,0,1024);
					sprintf(m_cs_encoding_adapter,"%s",t_gpu.gpu_name);
					m_encode_status = MW_ENCODEING;
					set_encode_status_text(m_cs_encode_status,m_encode_status);

					// create thread
					m_h_thread_encode = NULL;
					m_h_event_encode = NULL;
					m_h_event_exit_thread_encode = NULL;
					m_h_event_encode = CreateEvent(NULL,FALSE,FALSE,NULL);
					m_h_event_exit_thread_encode = CreateEvent(NULL,FALSE,FALSE,NULL);
					m_h_thread_encode = CreateThread(NULL,0,thread_encode,this,0,NULL);

					m_clock_start = clock();

					m_file_size.m_u32_gb = 0;
					m_file_size.m_u32_mb = 0;
					m_file_size.m_u32_kb = 0;
					m_file_size.m_u32_b = 0;
				}
				else{
					memset(m_cs_encoding_adapter,0,1024);
					sprintf(m_cs_encoding_adapter,"none");
					m_encode_status = MW_ENCODE_ERROR;
					set_encode_status_text(m_cs_encode_status,m_encode_status);
					m_p_menubar->set_menu_item_checked(m_cs_menu_file, m_cs_menu_start_record, false);
					m_p_menubar->set_menu_enable(m_cs_menu_device,true);


					if(m_p_file!=NULL){
						mw_mp4_close(m_p_file);
						m_p_file = NULL;
					}

					MessageBoxA(s_h_window,
						"This adapter doesn't support current encoding",
						"Warning",
						MB_OK);
				}
			}
		}
		else if(WAIT_OBJECT_0 + 1 == t_dw_ret){
			if(m_h_thread_encode!=NULL){
				SetEvent(m_h_event_exit_thread_encode);
				WaitForSingleObject(m_h_thread_encode,INFINITE);
				CloseHandle(m_h_thread_encode);
				m_h_thread_encode = NULL;
				CloseHandle(m_h_event_encode);
				m_h_event_encode = NULL;
				CloseHandle(m_h_event_exit_thread_encode);
				m_h_event_exit_thread_encode = NULL;
			}

			EnterCriticalSection(&m_cs_lock);

			memset(m_cs_encoding_adapter,0,1024);

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
				m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_start_record, true);
				m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_stop_record, false);
				m_p_menubar->set_menu_sub_menu_enable(m_cs_menu_file, m_cs_encoder_adapter, true);
				m_p_menubar->set_menu_enable(m_cs_menu_device,true);

				m_encode_status = MW_NOT_ENCODING;
				set_encode_status_text(m_cs_encode_status,m_encode_status);
			}
			else {
				m_encode_status = MW_ENCODE_STOPERROR;
				set_encode_status_text(m_cs_encode_status,m_encode_status);
				m_p_menubar->set_menu_item_checked(m_cs_menu_file, m_cs_menu_start_record, false);
				m_p_menubar->set_menu_enable(m_cs_menu_device,false);

				MessageBoxA(s_h_window,
					"Stop Encoding failed",
					"Warning",
					MB_OK);
			}
		}
		else if(WAIT_OBJECT_0 + 2 == t_dw_ret){
			if(m_n_capture_index!=m_n_ui_select){
				if(m_p_capture->get_is_captureing()){
					m_p_capture->stop_anc_capture();
					m_p_capture->stop_video_capture();
					m_capture_status = MW_NOT_CAPTUREING;
					set_capture_status_text(m_cs_capture_status,m_capture_status);
				}
				m_p_capture->close_channel();
			}

			if(m_n_ui_select==0){
				m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_start_record, false);
				m_p_menubar->set_menu_item_enable(m_cs_menu_file, m_cs_menu_stop_record, false);
				continue;
			}

			bool t_b_open = false;
			t_b_open = m_p_capture->open_channel(m_n_ui_select-1);
			if(!t_b_open){
				m_capture_status = MW_CAPTURE_ERROR;
				set_capture_status_text(
					m_cs_capture_status,
					m_capture_status);
				m_n_capture_index = -1;
				MessageBoxA(s_h_window,
					"Open Channel failed",
					"Warning",
					MB_OK);
				continue;
			}

			m_n_capture_index = m_n_ui_select;

			CMWRenderBuffer* t_p_render_buffer =NULL;
			t_p_render_buffer = m_p_opengl_render->get_render_buffer();
			if(m_n_capture_index != -1){
				m_p_capture->start_anc_capture(callback_anc_capture,this);
				bool t_b_capture = m_p_capture->start_video_capture(m_video_capture_param, callback_video_capture, this,t_p_render_buffer);
				if (t_b_capture) {
					m_capture_status = MW_CAPTUREING;
					set_capture_status_text(m_cs_capture_status,m_capture_status);
					if(!m_vec_gpu.empty()){
						m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_start_record,true);
						m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_stop_record,false);
					}
				}
				else{
					m_capture_status = MW_CAPTURE_ERROR;
					set_capture_status_text(m_cs_capture_status,m_capture_status);
					m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_start_record,false);
					m_p_menubar->set_menu_item_enable(m_cs_menu_file,m_cs_menu_stop_record,false);
					MessageBoxA(s_h_window,
						"Start Capture failed",
						"Warning",
						MB_OK);
				}
			}else{

			}
		}
		else if(WAIT_OBJECT_0 + 3 == t_dw_ret){
			// exit thread
			t_dw_re = 0;
			break;
		}
		else{
			// exit thread with error
			t_dw_ret = 1;
			break;
		}
	}

	return t_dw_re;
}

DWORD CMWCCPlayerUI::thread_encode_proc()
{
	DWORD t_dw_ret = 0;

	HANDLE t_array_handle[] = {m_h_event_encode,m_h_event_exit_thread_encode};

	bool t_b_running = true;
	while(t_b_running){
		DWORD t_dw_obj = WaitForMultipleObjects(2,t_array_handle,FALSE,INFINITE);
		if(WAIT_OBJECT_0 == t_dw_obj){
			if(NULL != m_p_encoder){
				EnterCriticalSection(&m_cs_lock);
				if(m_p_encoder->get_is_encoding()){
					m_p_encoder->put_frame_ex((uint8_t*)m_p_encode_buffer,m_u64_timestamp);
				}
				LeaveCriticalSection(&m_cs_lock);
			}
		}
		else if(WAIT_OBJECT_0 + 1 == t_dw_obj){
			t_b_running = false;
			break;
		}
		else{
			t_b_running = false;
			break;
		}
	}

	return t_dw_ret;
}

void 
CMWCCPlayerUI::callback_encode_proc(
	const uint8_t* t_p_frame, 
	uint32_t t_u32_frame_len, 
	mw_venc_frame_info_t* t_p_frame_info)
{
	m_n_encode_num++;
	m_u64_total_encode_num++;

	clock_t t_clock = clock();
	if(m_last_encode_clock == 0)
		m_last_encode_clock = clock();

	if(t_clock - m_last_encode_clock >= 1000){
		m_f_encode_fps = m_n_encode_num*1000*1.0/(t_clock-m_last_encode_clock);
		m_n_encode_num = 0;
		m_last_encode_clock = t_clock;
	}

	if(m_clock_start>0){
		clock_t t_clock_now = clock();
		if(t_clock_now-m_clock_start>CLOCKS_PER_SEC*60*60){
			EnterCriticalSection(&m_cs_lock_file);
			if(m_p_file!=NULL){
				if(t_p_frame_info->frame_type == MW_VENC_FRAME_TYPE_IDR){
					mw_mp4_close(m_p_file);
					m_p_file = NULL;
					m_clock_start = 0;
				}

				if(m_p_file == NULL){
					wchar_t t_wcs_file_name[1024]={0};
					m_n_file_name_index++;
					wsprintf(t_wcs_file_name,L"%s-%d.mp4",m_wcs_file_name_main,m_n_file_name_index);
					m_p_file = mw_mp4_open_ex(t_wcs_file_name);
					m_b_need_set_mp4 = true;
					m_b_set_anc_track = true;
					m_clock_start = clock();
				}

			}
			LeaveCriticalSection(&m_cs_lock_file);
		}

		EnterCriticalSection(&m_cs_lock_file);
		if(m_p_file!=NULL){
			if(m_b_need_set_mp4){
				mw_mp4_video_info_t video_track_info;
				memset(&video_track_info,0,sizeof(video_track_info));
				video_track_info.codec_type = MW_MP4_VIDEO_TYPE_H264;
				video_track_info.timescale = 1000;
				video_track_info.width = m_video_capture_param.m_n_width;
				video_track_info.height = m_video_capture_param.m_n_height;
				video_track_info.hevc.vps = NULL;
				video_track_info.hevc.vps_size = 0;
				video_track_info.hevc.sps = NULL;
				video_track_info.hevc.sps_size = 0;
				video_track_info.hevc.pps = NULL;
				video_track_info.hevc.pps_size = 0;
				mw_mp4_status_t t_status = mw_mp4_set_video(m_p_file, &video_track_info);
				m_b_need_set_mp4 = false;
			}

			mw_mp4_write_video(
				m_p_file, 
				(const unsigned char*)t_p_frame, 
				t_u32_frame_len, 
				t_p_frame_info->pts);

			m_u64_current_file_size += t_u32_frame_len;
			m_u64_total_encode_size += t_u32_frame_len;
		}
		LeaveCriticalSection(&m_cs_lock_file);
	}
}

mw_render_fourcc_t CMWCCPlayerUI::get_render_fmt_from_fourcc(DWORD t_dw_fourcc)
{
	mw_render_fourcc_t t_color = MW_RENDER_FOURCC_CNT;

	switch (t_dw_fourcc)
	{
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

mw_venc_fourcc_t CMWCCPlayerUI::get_encode_fmt_from_fourcc(DWORD t_dw_fourcc)
{
	mw_venc_fourcc_t t_color = MW_VENC_FOURCC_COUNT;

	switch (t_dw_fourcc)
	{
	case MWFOURCC_NV12:
		t_color = MW_VENC_FOURCC_NV12;
		break;
	case MWFOURCC_NV21:
		t_color = MW_VENC_FOURCC_NV21;
		break;
	case MWFOURCC_YV12:
		t_color = MW_VENC_FOURCC_YV12;
		break;
	case MWFOURCC_I420:
		t_color = MW_VENC_FOURCC_I420;
		break;
	case MWFOURCC_YUY2:
		t_color = MW_VENC_FOURCC_YUY2;
		break;
	case MWFOURCC_P010:
		t_color = MW_VENC_FOURCC_P010;
		break;
	case MWFOURCC_BGRA:
		t_color = MW_VENC_FOURCC_BGRA;
		break;
	case MWFOURCC_RGBA:
		t_color = MW_VENC_FOURCC_RGBA;
		break;
	case MWFOURCC_ARGB:
		t_color = MW_VENC_FOURCC_ARGB;
		break;
	case MWFOURCC_ABGR:
		t_color = MW_VENC_FOURCC_ABGR;
		break;
	default:
		break;
	}

	return t_color;
}

void CMWCCPlayerUI::enum_encoder()
{
	uint32_t t_u32_platforms = mw_venc_get_support_platfrom();

	mw_venc_handle_t t_h_venc = NULL;
	mw_venc_get_default_param(&m_venc_param);
	m_venc_param.width = 1920;
	m_venc_param.height = 1080;
	m_venc_param.fps.den = 1;
	m_venc_param.fps.num = 60;
	m_venc_param.code_type = MW_VENC_CODE_TYPE_H264;
	m_venc_param.fourcc = get_encode_fmt_from_fourcc(m_video_capture_param.m_dw_fourcc);
	m_venc_param.rate_control.mode = MW_VENC_RATECONTROL_CBR;
	m_venc_param.rate_control.target_bitrate = 4096;
	m_venc_param.gop_pic_size = 60;

	uint32_t t_u32_gpus = mw_venc_get_gpu_num();
	for(int i=0;i<t_u32_gpus;i++){
		mw_venc_gpu_info_t info;
		mw_venc_get_gpu_info_by_index(i, &info);
		m_vec_gpu.push_back(info);
		m_vec_index.push_back(i);
	}
}

void 
CMWCCPlayerUI::set_capture_status_text(
	char* t_cs_text,
	mw_capture_status_e t_status)
{
	switch (t_status)
	{
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

void 
CMWCCPlayerUI::set_encode_status_text(
	char* t_cs_text,
	mw_encode_status_e t_status)
{
	memset(t_cs_text, 0, 256);
	switch (t_status)
	{
	case MW_ENCODEING:
		sprintf(t_cs_text, "encode capturing");
		break;
	case MW_NOT_ENCODING:
		sprintf(t_cs_text, "encode stopped");
		break;
	case MW_ENCODE_ERROR:
		sprintf(t_cs_text, "This graphic adapter doesn't support P010.");
		break;
	case MW_ENCODE_ERROR_NVIDIA:
		sprintf(t_cs_text,"NVIDIA doesn't support YUY2 encoding.");
		break;
	case MW_ENCODE_STOPERROR:
		sprintf(t_cs_text, "encode stop error");
		break;
	default:
		sprintf(t_cs_text, "encode error");
		break;
	}
}

bool CMWCCPlayerUI::get_disk_size(mw_disk_size_t& t_size)
{
	bool t_b_ret = false;

	ULARGE_INTEGER t_free_bytes_available_to_caller = {0};
	ULARGE_INTEGER t_total_number_of_bytes = {0};
	ULARGE_INTEGER t_total_number_of_free_bytes = {0};

	t_b_ret = GetDiskFreeSpaceEx(
		m_wcs_dir,
		&t_free_bytes_available_to_caller,
		&t_total_number_of_bytes,
		&t_total_number_of_free_bytes);
	if(t_b_ret){
		uint64_t t_u64_freebytes_avaiable_to_user = t_free_bytes_available_to_caller.QuadPart;
		uint64_t t_u64_total_number_bytes = t_total_number_of_bytes.QuadPart;
		uint64_t t_u64_total_number_of_free_bytes = t_total_number_of_free_bytes.QuadPart;

		t_size.m_u32_b = t_u64_freebytes_avaiable_to_user&1023;
		t_size.m_u32_kb = (t_u64_freebytes_avaiable_to_user>>10)&1023;
		t_size.m_u32_mb = (t_u64_freebytes_avaiable_to_user>>20)&1023;
		t_size.m_u32_gb = (t_u64_freebytes_avaiable_to_user>>30);

	}

	return t_b_ret;
}

CMWCCPlayerUI::CMWCCPlayerUI()
{
	m_p_imgui_ctx = NULL;
	m_p_opengl_render = NULL;

	m_p_file = NULL;

	sprintf(m_cs_menu_file,"File");
	sprintf(m_cs_menu_start_record,"Start Record");
	sprintf(m_cs_menu_stop_record,"Stop Record");
	sprintf(m_cs_encoder_adapter,"Encode Adapter");

	m_capture_status = MW_NOT_CAPTUREING;
	m_encode_status = MW_NOT_ENCODING;

	m_f_capture_fps = 0;
	m_u64_current_file_size = 0;
	m_u64_total_capture_num = 0;
	m_u64_total_encode_num = 0;
	m_u64_total_encode_size = 0;
	m_f_encode_fps = 0;

	memset(m_wcs_file_name_sub,0,1024*2);
	wsprintf(m_wcs_file_name_sub,L"none");

	memset(m_cs_encoding_adapter,0,1024);
	sprintf(m_cs_encoding_adapter,"none");

	memset(m_cs_disk_size,0,256);
	sprintf(m_cs_disk_size,"not selected");

	m_capture_status = MW_NOT_CAPTUREING;
	memset(m_cs_capture_status,0,512);
	set_capture_status_text(m_cs_capture_status,m_capture_status);
	m_encode_status = MW_NOT_ENCODING;
	memset(m_cs_encode_status,0,512);
	set_encode_status_text(m_cs_encode_status,m_encode_status);

	memset(m_wcs_dir,0,256*2);
	wsprintf(m_wcs_dir,L"not selected");
	
	m_n_len_render = 256;
	memset(m_cs_render,0,m_n_len_render);

	m_p_encode_buffer = new char[4096*2160*4];

	m_h_event_encode = NULL;
	m_h_event_stop_record = NULL;
	m_h_event_exit_thread_encode = NULL;
	m_h_thread_encode = NULL;

	m_h_thread_cmd_proc = NULL;
	m_h_event_exit_thread = NULL;
	m_h_event_openfile = NULL;

	sprintf(m_cs_menu_device,"Devices");

	sprintf(m_cs_menu_cc_settings,"CC Settings");

	m_video_capture_param.m_n_width = 1920;
	m_video_capture_param.m_n_height = 1080;
	m_video_capture_param.m_dw_fourcc = MWFOURCC_NV12;
	m_video_capture_param.m_ul_frameduration = 166667;

	m_p_capture = new CMWCapture();
	m_p_capture->init_capture();
	m_p_capture->refresh_device();
	m_n_capture_index = -1;

	m_p_render_buffer = new CMWRenderBuffer();
	m_p_render_buffer->allocate_buffer(3840*2160*4);

	m_b_show_cc708 = true;
	m_b_show_cc608 = true;
	m_n_cc608_out = 0;

	m_p_cc708_decoder = MWCreateCC708Decoder();
	m_p_cc_render = MWCreateRender();
	MWSetCC708DecoderCallback(m_p_cc708_decoder,callback_anc_decode,this);
	MWSetCC708DecodeType(m_p_cc708_decoder,m_b_show_cc608, m_b_show_cc708);
	MWEnableOutputChannel(m_p_cc708_decoder, MW_CC_ALL);

	// init encode part
	mw_venc_init();

	// create encoder
	m_p_encoder = new CMWEncoder();

	InitializeCriticalSection(&m_cs_lock);
	InitializeCriticalSection(&m_cs_lock_cc);
	InitializeCriticalSection(&m_cs_lock_file);
}

CMWCCPlayerUI::~CMWCCPlayerUI()
{
	if(m_p_capture){
		m_p_capture->stop_anc_capture();
		m_p_capture->stop_video_capture();
		m_p_capture->deinit_capture();

		delete m_p_capture;
		m_p_capture = NULL;
	}

	if(m_p_render_buffer != NULL){
		delete m_p_render_buffer;
		m_p_render_buffer = NULL;
	}

	if(m_h_thread_encode!=NULL){
		SetEvent(m_h_event_exit_thread_encode);
		WaitForSingleObject(m_h_thread_encode,INFINITE);
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

	if(m_h_thread_cmd_proc){
		SetEvent(m_h_event_exit_thread);
		WaitForSingleObject(m_h_thread_cmd_proc,INFINITE);
		CloseHandle(m_h_thread_cmd_proc);
		m_h_thread_cmd_proc = NULL;
		CloseHandle(m_h_event_exit_thread);
		m_h_event_exit_thread = NULL;
	}

	if(m_h_event_openfile){
		CloseHandle(m_h_event_openfile);
		m_h_event_openfile = NULL;
	}

	if(m_h_event_stop_record){
		CloseHandle(m_h_event_stop_record);
		m_h_event_stop_record = NULL;
	}

	if(m_h_event_device_changed){
		CloseHandle(m_h_event_device_changed);
		m_h_event_device_changed = NULL;
	}

	if(m_p_encoder){
		delete m_p_encoder;
		m_p_encoder = NULL;
	}

	mw_venc_deinit();

	if(m_p_encode_buffer){
		delete m_p_encode_buffer;
		m_p_encode_buffer = NULL;
	}

	if(m_p_cc_screen){
		delete m_p_cc_screen;
		m_p_cc_screen = NULL;
	}

	DeleteCriticalSection(&m_cs_lock_file);
	DeleteCriticalSection(&m_cs_lock_cc);
	DeleteCriticalSection(&m_cs_lock);
}
