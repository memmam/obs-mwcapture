/************************************************************************************************/
// mw_imgui_widget_videoparam.cpp : implementation of the CMWIMGUIWidgetVideoParam class

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

#include "mw_imgui_widget_videoparam.h"
#include "stdio.h"

CMWIMGUIWidgetVideoParam::CMWIMGUIWidgetVideoParam(CMWIMGUIUIManager * t_p_manager):
	CMWIMGUIWidget(t_p_manager)
{
	// init params
	m_n_color_num = 10;
	m_n_ticks_per_second = 10000000;
	m_callback_change = NULL;
	m_p_caller = NULL;

	m_video_capture_param.m_n_width = 1920;
	m_video_capture_param.m_n_height = 1080;
	m_video_capture_param.m_n_frameduration = 166667;
	m_video_capture_param.m_u32_color = MWFOURCC_YUY2;

	m_n_width = m_video_capture_param.m_n_width;
	m_n_height = m_video_capture_param.m_n_height;
	m_n_selected_color = 0;
	m_n_frameduration = m_video_capture_param.m_n_frameduration;
	m_n_count_format = 0;

	m_n_num_duration = 48;
	m_n_len_duration = 24;
	m_n_count_duration = 0;
	m_pp_cs_duration = new char*[m_n_num_duration];
	if(m_pp_cs_duration != NULL){
		for(int i=0;i<m_n_num_duration;i++){
			m_pp_cs_duration[i] = NULL;
			m_pp_cs_duration[i] = new char[m_n_len_duration];
			if(m_pp_cs_duration[i]!=NULL)
				memset(m_pp_cs_duration[i],0,m_n_len_duration);
		}
	}
	m_vec_duration.clear();

	m_n_num_4krgba_duration = 48;
	m_n_len_4krgba_duration = 24;
	m_n_count_4krgba_duration = 0;
	m_pp_cs_4krgba_duration = new char*[m_n_num_4krgba_duration];
	if (m_pp_cs_4krgba_duration != NULL) {
		for (int i = 0;i < m_n_num_4krgba_duration;i++) {
			m_pp_cs_4krgba_duration[i] = NULL;
			m_pp_cs_4krgba_duration[i] = new char[m_n_len_4krgba_duration];
			if (m_pp_cs_4krgba_duration[i] != NULL)
				memset(m_pp_cs_4krgba_duration[i], 0, m_n_len_4krgba_duration);
		}
	}

	m_n_num_resolution = 48;
	m_n_len_resolution = 24;
	m_n_count_resolution = 0;
	m_pp_cs_resolution = new char*[m_n_num_resolution];
	if(m_pp_cs_resolution!=NULL){
		for(int i=0;i<m_n_num_resolution;i++){
			m_pp_cs_resolution[i]=NULL;
			m_pp_cs_resolution[i]= new char[m_n_len_resolution];
			if(m_pp_cs_resolution[i]!=NULL)
				memset(m_pp_cs_resolution[i],0,m_n_len_resolution);
		}
	}
	m_vec_resolution.clear();

	m_n_num_eco_bgra_resolution = 48;
	m_n_len_eco_bgra_resolution = 24;
	m_n_count_eco_bgra_resolution = 0;
	m_pp_cs_eco_bgra_resolution = new char*[m_n_num_eco_bgra_resolution];
	if(m_pp_cs_eco_bgra_resolution!=NULL){
		for(int i=0;i<m_n_num_eco_bgra_resolution;i++){
			m_pp_cs_eco_bgra_resolution[i]=NULL;
			m_pp_cs_eco_bgra_resolution[i] = new char[m_n_len_eco_bgra_resolution];
			if(m_pp_cs_eco_bgra_resolution[i]!=NULL)
				memset(m_pp_cs_eco_bgra_resolution[i],0,m_n_len_eco_bgra_resolution);
		}
	}
	m_vec_eco_bgra_resolution.clear();

	m_n_num_format = 48;
	m_n_len_format = 24;
	m_n_count_format = 0;
	m_pp_cs_format = new char*[m_n_num_format];
	if(m_pp_cs_format!=NULL){
		for(int i =0;i<m_n_num_format;i++){
			m_pp_cs_format[i]=NULL;
			m_pp_cs_format[i]=new char[m_n_len_format];
			if(m_pp_cs_format[i]!=NULL)
				memset(m_pp_cs_format[i],0,m_n_len_format);
		}
	}
	m_vec_format.clear();

	m_n_num_format_lack_bgra = 48;
	m_n_len_format_lack_bgra = 24;
	m_n_count_format_lack_bgra = 0;
	m_pp_cs_format_lack_bgra = new char*[m_n_num_format_lack_bgra];
	if(m_pp_cs_format_lack_bgra!=NULL){
		for(int i=0;i<m_n_num_format_lack_bgra;i++){
			m_pp_cs_format_lack_bgra[i]=NULL;
			m_pp_cs_format_lack_bgra[i]= new char[m_n_num_format_lack_bgra];
			if(m_pp_cs_format_lack_bgra[i]!=NULL)
				memset(m_pp_cs_format_lack_bgra[i],0,m_n_len_format_lack_bgra);
		}
	}

	m_device_family = MW_FAMILY_ID_PRO_CAPTURE;
}

CMWIMGUIWidgetVideoParam::~CMWIMGUIWidgetVideoParam()
{
	if(m_pp_cs_format!=NULL){
		delete []m_pp_cs_format;
		m_pp_cs_format = NULL;
	}

	if(m_pp_cs_duration!=NULL){
		delete []m_pp_cs_duration;
		m_pp_cs_duration = NULL;
	}

	if (m_pp_cs_4krgba_duration != NULL) {
		delete[]m_pp_cs_4krgba_duration;
		m_pp_cs_4krgba_duration = NULL;
	}

	if(m_pp_cs_resolution!=NULL){
		delete []m_pp_cs_resolution;
		m_pp_cs_resolution = NULL;
	}
}

void CMWIMGUIWidgetVideoParam::update_widget()
{
	if((m_u32_color==MWFOURCC_BGRA||
	   m_u32_color == MWFOURCC_RGBA)&&
	   (m_device_family==MW_FAMILY_ID_ECO_CAPTURE||m_device_family == MW_FAMILY_ID_USB_CAPTURE)){
		   ImGui::Combo("resolution",&m_n_selected_resolution_eco_bgra,m_pp_cs_eco_bgra_resolution,m_n_count_eco_bgra_resolution);
		   if(m_vec_eco_bgra_resolution.size()>m_n_selected_resolution_eco_bgra){
			   m_n_width = m_vec_eco_bgra_resolution.at(m_n_selected_resolution_eco_bgra).cx;
			   m_n_height = m_vec_eco_bgra_resolution.at(m_n_selected_resolution_eco_bgra).cy;
		   }
	}
	else{
		ImGui::Combo("resolution",&m_n_selected_resolution,m_pp_cs_resolution,m_n_count_resolution);
		if(m_vec_resolution.size()>m_n_selected_resolution){
			m_n_width = m_vec_resolution.at(m_n_selected_resolution).cx;
			m_n_height = m_vec_resolution.at(m_n_selected_resolution).cy;
		}
	}

	if (m_n_width == 3840 && m_n_height == 2160 && (m_u32_color == MWFOURCC_BGRA||m_u32_color == MWFOURCC_RGBA)) {
		ImGui::Combo("fps", &m_n_selected_4krgba_frameduration, m_pp_cs_4krgba_duration, m_n_count_4krgba_duration);
		if (m_vec_4krgba_duration.size() > m_n_selected_4krgba_frameduration)
			m_n_frameduration = m_vec_4krgba_duration.at(m_n_selected_4krgba_frameduration);
	}
	else {
		ImGui::Combo("fps", &m_n_selected_frameduration, m_pp_cs_duration, m_n_count_duration);
		if (m_vec_duration.size() > m_n_selected_frameduration)
			m_n_frameduration = m_vec_duration.at(m_n_selected_frameduration);
	}
		
	if(m_n_count_format>0){
		if((m_n_width>1920&&m_n_height>1080)&&
			(m_n_frameduration<333333||m_device_family == MW_FAMILY_ID_ECO_CAPTURE)){
				ImGui::Combo("color",&m_n_selected_color_lack_bgra, m_pp_cs_format_lack_bgra, m_n_count_format_lack_bgra);
				if(m_vec_format_lack_bgra.size()>m_n_selected_color_lack_bgra)
					m_u32_color = m_vec_format_lack_bgra.at(m_n_selected_color_lack_bgra);
		}
		else{
			ImGui::Combo("color",&m_n_selected_color, m_pp_cs_format, m_n_count_format);
			if(m_vec_format.size()>m_n_selected_color)
				m_u32_color = m_vec_format.at(m_n_selected_color);
		}	

		bool t_b_clicked_ok = ImGui::MWButton("  OK  ",ImVec2(0,0),true);
		ImGui::SameLine();
		ImGui::Text("               ");
		ImGui::SameLine();
		bool t_b_clicked_cancel = ImGui::MWButton("Cancel",ImVec2(0,0),true);
		if (t_b_clicked_ok) {
			printf("but ok clicked\n");

			m_video_capture_param.m_n_width = m_n_width;
			m_video_capture_param.m_n_height = m_n_height;
			m_video_capture_param.m_n_frameduration = m_n_frameduration;
			m_video_capture_param.m_u32_color = m_u32_color;

			if (m_callback_change != NULL)
				m_callback_change(m_video_capture_param, m_p_caller);
		}
		else {
			if (t_b_clicked_cancel) {
				printf("but cancel clicked\n");

				MWSIZE t_size;
				t_size.cx = m_video_capture_param.m_n_width;
				t_size.cy = m_video_capture_param.m_n_height;
				m_n_selected_resolution = get_index_resolution(t_size);
				m_n_selected_resolution_eco_bgra = get_index_resolution_eco_bgra(t_size);
				m_n_selected_frameduration = get_index_frameduration(m_video_capture_param.m_n_frameduration);
				m_n_selected_4krgba_frameduration = get_index_frameduration_4krgba(m_video_capture_param.m_n_frameduration);
				m_n_selected_color = get_index_color(m_video_capture_param.m_u32_color);
				m_n_selected_color_lack_bgra = get_index_color_lack_usb(m_video_capture_param.m_u32_color);

			}
		}
	}
	else{
		ImGui::Text("No supported color format.Please reconfigure the device.\n");
	}
	
}

void CMWIMGUIWidgetVideoParam::reset_ui()
{
	MWSIZE t_size;
	t_size.cx = m_video_capture_param.m_n_width;
	t_size.cy = m_video_capture_param.m_n_height;
	m_n_selected_resolution = get_index_resolution(t_size);
	m_n_selected_resolution_eco_bgra = get_index_resolution_eco_bgra(t_size);
	m_n_selected_frameduration = get_index_frameduration(m_video_capture_param.m_n_frameduration);
	m_n_selected_4krgba_frameduration = get_index_frameduration_4krgba(m_video_capture_param.m_n_frameduration);
	m_n_selected_color = get_index_color(m_video_capture_param.m_u32_color);
	m_n_selected_color_lack_bgra = get_index_color_lack_usb(m_video_capture_param.m_u32_color);
}

cmw_ui_video_capture_param_s CMWIMGUIWidgetVideoParam::get_ui_video_capture_param()
{
	return m_video_capture_param;
}

void CMWIMGUIWidgetVideoParam::set_param_change_callback(CALLBACK_PARAM_CHANGE t_callback, void* t_p_param)
{
	m_callback_change = t_callback;
	m_p_caller = t_p_param;
}

void CMWIMGUIWidgetVideoParam::set_supported_resolution(vector<MWSIZE> t_vec_resolution,MW_FAMILY_ID t_family_id)
{
	m_device_family = t_family_id;

	m_vec_resolution.clear();
	m_vec_eco_bgra_resolution.clear();
	m_vec_resolution = t_vec_resolution;
	for (int i = 0;i < m_vec_resolution.size();i++) {
		memset(m_pp_cs_resolution[i], 0, m_n_len_resolution);
		sprintf(m_pp_cs_resolution[i], "%dx%d", m_vec_resolution.at(i).cx, m_vec_resolution.at(i).cy);
		if(m_device_family == MW_FAMILY_ID_ECO_CAPTURE||m_device_family == MW_FAMILY_ID_USB_CAPTURE){
			int t_n_size = m_vec_eco_bgra_resolution.size();
			if(m_vec_resolution.at(i).cx<=1920&&m_vec_resolution.at(i).cy<=1080){
				sprintf(m_pp_cs_eco_bgra_resolution[t_n_size], "%dx%d", m_vec_resolution.at(i).cx, m_vec_resolution.at(i).cy);
				m_vec_eco_bgra_resolution.push_back(m_vec_resolution.at(i));
			}
		}
	}
	m_n_count_resolution = m_vec_resolution.size();
	m_n_selected_resolution = 0;
	m_n_width = m_vec_resolution.at(0).cx;
	m_n_height = m_vec_resolution.at(0).cy;
	m_video_capture_param.m_n_width = m_n_width;
	m_video_capture_param.m_n_height = m_n_height;

	m_n_count_eco_bgra_resolution = m_vec_eco_bgra_resolution.size();
	m_n_selected_resolution_eco_bgra = 0;
}

void CMWIMGUIWidgetVideoParam::set_supported_frameduration(vector<uint64_t> t_vec_duration,MW_FAMILY_ID t_family_id)
{
	m_device_family = t_family_id;

	m_vec_duration.clear();
	m_vec_duration = t_vec_duration;
	m_vec_4krgba_duration.clear();
	for(int i=0;i<m_vec_duration.size();i++){
		memset(m_pp_cs_duration[i],0,m_n_len_duration);
		sprintf(m_pp_cs_duration[i],"%.2f",m_n_ticks_per_second * 1.0/t_vec_duration.at(i));

		if (m_vec_duration[i] >= 333333) {
			int t_n_size = m_vec_4krgba_duration.size();
			memset(m_pp_cs_4krgba_duration[t_n_size], 0, m_n_len_4krgba_duration);
			sprintf(m_pp_cs_4krgba_duration[t_n_size], "%.2f", m_n_ticks_per_second * 1.0 / t_vec_duration.at(i));
			m_vec_4krgba_duration.push_back(m_vec_duration[i]);
		}
	}

	m_n_count_duration = m_vec_duration.size();
	m_n_selected_frameduration = 0;
	m_n_frameduration = m_vec_duration.at(0);
	m_video_capture_param.m_n_frameduration = m_n_frameduration;

	m_n_selected_4krgba_frameduration = 0;
	m_n_count_4krgba_duration = m_vec_4krgba_duration.size();
}

void CMWIMGUIWidgetVideoParam::set_supported_colorformat(vector<uint32_t> t_vec_fourcc,MW_FAMILY_ID t_family_id)
{
	m_device_family = t_family_id;

	m_vec_format = t_vec_fourcc;
	m_vec_format_lack_bgra.clear();
	for(int i=0;i<m_vec_format.size();i++){
		memset(m_pp_cs_format[i],0,m_n_len_format);
		trans_fourcc(m_pp_cs_format[i],m_vec_format.at(i));
		if(m_vec_format.at(i)!=MWFOURCC_BGRA&&m_vec_format.at(i)!=MWFOURCC_RGBA){
			int t_n_size = m_vec_format_lack_bgra.size();
			trans_fourcc(m_pp_cs_format_lack_bgra[t_n_size],m_vec_format.at(i));
			m_vec_format_lack_bgra.push_back(m_vec_format.at(i));
		}
	}
	m_n_count_format = m_vec_format.size();
	m_n_selected_color = 0;
	if(m_n_count_format>0)
		m_u32_color = m_vec_format.at(0);
	else
		m_u32_color = 0;

	m_video_capture_param.m_u32_color = m_u32_color;

	m_n_count_format_lack_bgra = m_vec_format_lack_bgra.size();
	m_n_selected_color_lack_bgra = 0;
}

void CMWIMGUIWidgetVideoParam::trans_fourcc(char* t_cs_fourcc,uint32_t t_u32_fourcc)
{
	switch (t_u32_fourcc)
	{
	case MWFOURCC_GREY:
		sprintf(t_cs_fourcc,"GREY");
		break;
	case MWFOURCC_Y800:
		sprintf(t_cs_fourcc,"Y800");
		break;
	case MWFOURCC_Y8:
		sprintf(t_cs_fourcc,"Y8");
		break;
	case MWFOURCC_Y16:
		sprintf(t_cs_fourcc,"Y16");
		break;
	case MWFOURCC_RGB15:
		sprintf(t_cs_fourcc,"RGB15");
		break;
	case MWFOURCC_RGB16:
		sprintf(t_cs_fourcc,"RGB16");
		break;
	case MWFOURCC_RGB24:
		sprintf(t_cs_fourcc,"RGB16");
		break;
	case MWFOURCC_RGBA:
		sprintf(t_cs_fourcc,"RGBA");
		break;
	case MWFOURCC_BGRA:
		sprintf(t_cs_fourcc,"BGRA");
		break;
	case MWFOURCC_ABGR:
		sprintf(t_cs_fourcc,"ABGR");
		break;
	case MWFOURCC_ARGB:
		sprintf(t_cs_fourcc,"ARGB");
		break;
	case MWFOURCC_NV16:
		sprintf(t_cs_fourcc,"NV16");
		break;
	case MWFOURCC_NV61:
		sprintf(t_cs_fourcc,"NV61");
		break;
	case MWFOURCC_I422:
		sprintf(t_cs_fourcc,"I422");
		break;
	case MWFOURCC_YV16:
		sprintf(t_cs_fourcc,"YV16");
		break;
	case MWFOURCC_YUY2:
		sprintf(t_cs_fourcc,"YUY2");
		break;
	case MWFOURCC_YUYV:
		sprintf(t_cs_fourcc,"YUYV");
		break;
	case MWFOURCC_UYVY:
		sprintf(t_cs_fourcc,"UYVY");
		break;
	case MWFOURCC_YVYU:
		sprintf(t_cs_fourcc,"YVYU");
		break;
	case MWFOURCC_VYUY:
		sprintf(t_cs_fourcc,"VYUY");
		break;
	case MWFOURCC_I420:
		sprintf(t_cs_fourcc,"I420");
		break;
	case MWFOURCC_IYUV:
		sprintf(t_cs_fourcc,"IYUV");
		break;
	case MWFOURCC_NV12:
		sprintf(t_cs_fourcc,"NV12");
		break;
	case MWFOURCC_YV12:
		sprintf(t_cs_fourcc,"YV12");
		break;
	case MWFOURCC_NV21:
		sprintf(t_cs_fourcc,"NV21");
		break;
	case MWFOURCC_P010:
		sprintf(t_cs_fourcc,"P010");
		break;
	case MWFOURCC_P210:
		sprintf(t_cs_fourcc,"P210");
		break;
	case MWFOURCC_IYU2:
		sprintf(t_cs_fourcc,"IYU2");
		break;
	case MWFOURCC_V308:
		sprintf(t_cs_fourcc,"V308");
		break;
	case MWFOURCC_AYUV:
		sprintf(t_cs_fourcc,"AYUV");
		break;
	case MWFOURCC_UYVA:
		sprintf(t_cs_fourcc,"UYVA");
		break;
	case MWFOURCC_V408:
		sprintf(t_cs_fourcc,"V408");
		break;
	case MWFOURCC_VYUA:
		sprintf(t_cs_fourcc,"VYUA");
		break;
	//case MWFOURCC_V210:
	//	sprintf(t_cs_fourcc,"V210");
	//	break;
	case MWFOURCC_Y410:
		sprintf(t_cs_fourcc,"Y410");
		break;
	case MWFOURCC_V410:
		sprintf(t_cs_fourcc,"V410");
		break;
	case MWFOURCC_RGB10:
		sprintf(t_cs_fourcc,"RGB10");
		break;
	case MWFOURCC_BGR10:
		sprintf(t_cs_fourcc,"BGR10");
		break;
	default:
		break;
	}
}

int CMWIMGUIWidgetVideoParam::get_index_resolution(MWSIZE t_size)
{
	int t_n_index = 0;

	for(int i=0;i<m_vec_resolution.size();i++){
		if(m_vec_resolution.at(i).cx == t_size.cx && m_vec_resolution.at(i).cy == t_size.cy){
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

int CMWIMGUIWidgetVideoParam::get_index_resolution_eco_bgra(MWSIZE t_size)
{
	int t_n_index = 0;

	for(int i=0;i<m_vec_eco_bgra_resolution.size();i++){
		if(m_vec_eco_bgra_resolution.at(i).cx == t_size.cx && m_vec_eco_bgra_resolution.at(i).cy == t_size.cy){
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

int CMWIMGUIWidgetVideoParam::get_index_frameduration(uint64_t t_u64_duration)
{
	int t_n_index = 0;

	for(int i=0;i<m_vec_duration.size();i++){
		if(t_u64_duration == m_vec_duration.at(i)){
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

int CMWIMGUIWidgetVideoParam::get_index_frameduration_4krgba(uint64_t t_u64_duration)
{
	int t_n_index = 0;

	for (int i = 0;i < m_vec_4krgba_duration.size();i++) {
		if (t_u64_duration == m_vec_4krgba_duration.at(i)) {
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

int CMWIMGUIWidgetVideoParam::get_index_color(uint32_t t_u32_color)
{
	int t_n_index = 0;

	for(int i=0;i<m_vec_format.size();i++){
		if(t_u32_color == m_vec_format.at(i)){
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

int CMWIMGUIWidgetVideoParam::get_index_color_lack_usb(uint32_t t_u32_color)
{
	int t_n_index = 0;

	for(int i=0;i<m_vec_format_lack_bgra.size();i++){
		if(t_u32_color == m_vec_format_lack_bgra.at(i)){
			t_n_index = i;
			break;
		}
	}

	return t_n_index;
}

