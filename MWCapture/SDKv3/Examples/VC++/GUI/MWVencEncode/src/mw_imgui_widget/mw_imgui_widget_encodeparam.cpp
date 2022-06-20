/************************************************************************************************/
// mw_imgui_widget_encodeparam.cpp : implementation of the CMWIMGUIWidgetEncodeParam class

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

#include "mw_imgui_widget_encodeparam.h"

const char *g_cs_platforms[] = {"AMD", "INTEL", "NVIDIA"};
const char *g_cs_amd_mem[] = {"Auto", "DX11", "DX9", "CPU"};
const char *g_cs_target_usage[] = {"Best Speed", "Balanced", "Best Quality"};
const char *g_cs_rate_control[] = {"CBR", "VBR", "CQP"};
const char *g_cs_h264_profile[] = {"H264 Baseline", "H264 Main", "H264 High"};
const char *g_cs_h265_profile[] = {"H265 Main"};
const char *g_cs_level[] = {"level 2_1", "level 3_1", "level 4_1", "level 5_1",
			    "level 5_2", "level 6_1", "level 6_2"};
const char *g_cs_fourcc[] = {"NV12", "NV21", "YV12", "I420", "YUY2",
			     "P010", "BGRA", "RGBA", "ARGB", "ABGR"};

CMWIMGUIWidgetEncodeParam::CMWIMGUIWidgetEncodeParam(
	CMWIMGUIUIManager *t_p_manager)
	: CMWIMGUIWidget(t_p_manager)
{
	m_n_adapter_selected = 0;
	m_n_amd_mem_selected = 0;
	m_n_encode_type = 0;
	m_n_target_usage_selected = 0;
	m_n_rate_control_selected = 0;

	m_n_target_bitrate = 4096;
	m_n_max_bitrate = 4096;

	m_n_i = 26;
	m_n_p = 26;
	m_n_b = 26;

	mw_venc_get_default_param(&m_venc_param);

	m_n_slice_num = 1;

	m_n_gop_size = 60;

	m_n_gop_ref_size = 1;

	m_n_profile_h264_selected = 0;
	m_n_profile_h265_selected = 0;

	m_n_level_selected = 4; // default level 5_2

	m_n_width = 1920;
	m_n_height = 1080;
	m_vf_fourcc = MW_VENC_FOURCC_YUY2;
	m_n_fps_den = 1;
	m_n_fps_num = 60;

	memset(m_venc_gpu.gpu_name, 0, 128);
	sprintf(m_venc_gpu.gpu_name, "default");
	m_venc_gpu.platform = MW_VENC_PLATFORM_AMD;
	m_venc_platform = MW_VENC_PLATFORM_AMD;
	if (m_venc_platform == MW_VENC_PLATFORM_AMD)
		m_venc_param.amd_mem_reserved =
			get_amd_mem_type_from_selected(m_n_amd_mem_selected);

	m_venc_param.code_type = get_code_type_from_selected(m_n_encode_type);
	m_venc_param.fourcc = m_vf_fourcc;
	m_venc_param.fps.den = m_n_fps_den;
	m_venc_param.fps.num = m_n_fps_num;
	m_venc_param.gop_pic_size = m_n_gop_size;
	m_venc_param.gop_ref_size = m_n_gop_ref_size;
	m_venc_param.width = m_n_width;
	m_venc_param.height = m_n_height;
	m_venc_param.level = get_level_from_selected(m_n_level_selected);
	bool t_b_h264 =
		m_venc_param.code_type == MW_VENC_CODE_TYPE_H264 ? true : false;
	m_venc_param.profile =
		get_profile_from_selected(m_n_profile_h264_selected, t_b_h264);
	m_venc_param.rate_control.mode =
		get_rate_control_mode_from_selected(m_n_rate_control_selected);
	if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CBR)
		m_venc_param.rate_control.target_bitrate = m_n_target_bitrate;
	else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_VBR) {
		m_venc_param.rate_control.target_bitrate = m_n_target_bitrate;
		m_venc_param.rate_control.max_bitrate = m_n_max_bitrate;
	} else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CQP) {
		m_venc_param.rate_control.qpi = m_n_i;
		m_venc_param.rate_control.qpb = m_n_b;
		m_venc_param.rate_control.qpp = m_n_p;
	}
	m_venc_param.slice_num = m_n_slice_num;

	m_pp_cs_adapters = NULL;
	m_n_num_platform = 32;
	m_n_len_platform = 128;
	m_n_count_adapters = 0;
	m_pp_cs_adapters = new char *[m_n_num_platform];
	if (m_pp_cs_adapters != NULL) {
		for (int i = 0; i < m_n_num_platform; i++) {
			m_pp_cs_adapters[i] = NULL;
			m_pp_cs_adapters[i] = new char[m_n_len_platform];
			if (m_pp_cs_adapters[i] != NULL)
				memset(m_pp_cs_adapters[i], 0,
				       m_n_len_platform);
		}
	}

	m_callback_enable_encode = NULL;
	m_p_caller = NULL;
}

CMWIMGUIWidgetEncodeParam::~CMWIMGUIWidgetEncodeParam() {}

void CMWIMGUIWidgetEncodeParam::update_widget()
{
	// platform
	if (m_vec_gpu.size() > 0) {

		ImGui::Combo("Adapters", &m_n_adapter_selected,
			     m_pp_cs_adapters, m_n_count_adapters);
		if (m_vec_gpu.at(m_n_adapter_selected).platform ==
		    MW_VENC_PLATFORM_AMD) {
			ImGui::Combo("amd mem", &m_n_amd_mem_selected,
				     g_cs_amd_mem, 4);
		}
		if (m_vec_gpu.at(m_n_adapter_selected).platform ==
			    MW_VENC_PLATFORM_NVIDIA &&
		    m_vf_fourcc == MW_VENC_FOURCC_YUY2) {
			ImGui::Text("Nvidia doesn't support YUY2.");
			if (m_callback_enable_encode != NULL)
				m_callback_enable_encode(false, m_p_caller);
			return;
		} else {
			if (m_callback_enable_encode != NULL)
				m_callback_enable_encode(true, m_p_caller);
		}

		ImGui::Separator();

		// encode type
		ImGui::RadioButton("H264", &m_n_encode_type, 0);
		ImGui::SameLine();
		ImGui::RadioButton("H265", &m_n_encode_type, 1);

		ImGui::Separator();

		// fourcc
		// same with capture
		ImGui::LabelText("fourcc", "%s",
				 get_fourcc_from_vf_fourcc(m_vf_fourcc));

		ImGui::Separator();

		// target usage
		ImGui::Combo("target uasge", &m_n_target_usage_selected,
			     g_cs_target_usage, 3);

		ImGui::Separator();

		// rate control
		// cbr vbr cqp
		ImGui::Combo("rate control", &m_n_rate_control_selected,
			     g_cs_rate_control, 3);
		if (m_n_rate_control_selected == 0) {
			// cbr: constant bit rate
			ImGui::InputInt("target bitrate", &m_n_target_bitrate);
			if (m_n_target_bitrate < 100 ||
			    m_n_target_bitrate > 100000)
				m_n_target_bitrate = 4096;

		} else if (m_n_rate_control_selected == 1) {
			// vbr: variable bit rate
			ImGui::InputInt("target bitrate", &m_n_target_bitrate);
			if (m_n_target_bitrate < 100 ||
			    m_n_target_bitrate > 100000)
				m_n_target_bitrate = 4096;
			ImGui::InputInt("max bitrate", &m_n_max_bitrate);
			if (m_n_max_bitrate < 100 || m_n_max_bitrate > 100000)
				m_n_target_bitrate = 4096;
			if (m_n_max_bitrate < m_n_target_bitrate)
				m_n_max_bitrate = m_n_target_bitrate;
		} else {
			// cqp: const q p
			ImGui::InputInt("I", &m_n_i);
			if (m_n_i <= 0 || m_n_i > 51)
				m_n_i = 26;
			ImGui::InputInt("P", &m_n_p);
			if (m_n_p <= 0 || m_n_p > 51)
				m_n_p = 26;
			ImGui::InputInt("B", &m_n_b);
			if (m_n_b <= 0 || m_n_b > 51)
				m_n_b = 26;
		}

		ImGui::Separator();

		// width
		// same with capture
		ImGui::LabelText("width", "%d", m_n_width);

		// height
		// same with capture
		ImGui::LabelText("height", "%d", m_n_height);

		ImGui::Separator();

		// fps
		// same with capture
		ImGui::LabelText("fps", "%.2f %d/%d",
				 m_n_fps_num * 1.0 / m_n_fps_den, m_n_fps_num,
				 m_n_fps_den);

		ImGui::Separator();

		// slice num
		ImGui::InputInt("slice num", &m_n_slice_num);
		if (m_n_slice_num <= 0 || m_n_slice_num > 32)
			m_n_slice_num = 1;

		ImGui::Separator();

		// gop size
		ImGui::InputInt("gop size", &m_n_gop_size);
		if (m_n_gop_size <= 0 || m_n_gop_size > 1000)
			m_n_gop_size = 30;

		ImGui::Separator();

		// gop ref size
		//ImGui::InputInt("gop ref size", &m_n_gop_ref_size);
		//if(m_n_gop_ref_size<0||m_n_gop_ref_size>8)
		//	m_n_gop_ref_size = 1;
		m_n_gop_ref_size = 1;

		ImGui::Separator();

		// profile
		if (m_n_encode_type == 0) {
			ImGui::Combo("H264 profile", &m_n_profile_h264_selected,
				     g_cs_h264_profile, 3);
		} else if (m_n_encode_type == 1) {
			ImGui::Combo("H265 profile", &m_n_profile_h265_selected,
				     g_cs_h265_profile, 1);
		}

		ImGui::Separator();

		// level
		//ImGui::Combo("level", &m_n_level_selected, g_cs_level, 7);
		m_n_level_selected = 4;

		ImGui::Separator();

		// check for change
		bool t_b_change = check_param_change();

		// button ok & cancel
		bool t_b_clicked_ok = ImGui::Button("  OK  ");
		ImGui::SameLine();
		ImGui::Text("                         ");
		ImGui::SameLine();
		bool t_b_clicked_cancel = ImGui::Button("Cancel");
		if (t_b_clicked_ok) {
			if (t_b_change)
				set_venc_param();
		} else {
			if (t_b_clicked_cancel) {
				if (t_b_change)
					set_ui_param();
			}
		}
	} else {
		ImGui::Text(
			"Graphic Adapters on this machine don't support Hardware Encoding.");
	}
}

void CMWIMGUIWidgetEncodeParam::reset_ui()
{
	set_ui_param();
}

void CMWIMGUIWidgetEncodeParam::set_enable_encode_callback(
	CALLBACK_ENABLE_ENCODE t_callback, void *t_p_param)
{
	m_callback_enable_encode = t_callback;
	m_p_caller = t_p_param;
}

mw_venc_param_t CMWIMGUIWidgetEncodeParam::get_encode_param()
{
	return m_venc_param;
}

mw_venc_gpu_info_t CMWIMGUIWidgetEncodeParam::get_encode_adapter()
{
	m_venc_gpu = get_adapter_from_selected(m_n_adapter_selected);
	return m_venc_gpu;
}

int CMWIMGUIWidgetEncodeParam::get_encoder_index()
{
	return m_vec_index.at(m_n_adapter_selected);
}

void CMWIMGUIWidgetEncodeParam::set_video_base(cmw_video_base_s t_base)
{
	m_n_width = t_base.m_n_width;
	m_n_height = t_base.m_n_height;
	m_vf_fourcc = t_base.m_vf_fourcc;
	m_n_fps_den = t_base.m_n_fps_den;
	m_n_fps_num = t_base.m_n_fps_num;

	m_venc_param.width = m_n_width;
	m_venc_param.height = m_n_height;
	m_venc_param.fourcc = m_vf_fourcc;
	m_venc_param.fps.den = m_n_fps_den;
	m_venc_param.fps.num = m_n_fps_num;
}

void CMWIMGUIWidgetEncodeParam::init_support_platform(
	vector<mw_venc_platform_t> t_vec_platform,
	vector<mw_venc_gpu_info> t_vec_gpu, vector<int> t_vec_index)
{
	m_vec_platform = t_vec_platform;

	bool t_b_amd = false;
	bool t_b_intel = false;
	bool t_b_nvidia = false;
	for (int i = 0; i < m_vec_platform.size(); i++) {
		if (m_vec_platform.at(i) == MW_VENC_PLATFORM_AMD) {
			t_b_amd = true;
		} else if (m_vec_platform.at(i) == MW_VENC_PLATFORM_INTEL) {
			t_b_intel = true;
		} else if (m_vec_platform.at(i) == MW_VENC_PLATFORM_NVIDIA) {
			t_b_nvidia = true;
		}
	}

	for (int i = 0; i < t_vec_gpu.size(); i++) {
		if ((t_vec_gpu.at(i).platform == MW_VENC_PLATFORM_AMD &&
		     t_b_amd) ||
		    (t_vec_gpu.at(i).platform == MW_VENC_PLATFORM_INTEL &&
		     t_b_intel) ||
		    (t_vec_gpu.at(i).platform == MW_VENC_PLATFORM_NVIDIA &&
		     t_b_nvidia)) {
			m_vec_gpu.push_back(t_vec_gpu.at(i));
			m_vec_index.push_back(t_vec_index.at(i));
		}
	}

	// max 32 gpus
	m_n_count_adapters = m_vec_gpu.size();
	for (int i = 0; i < m_n_count_adapters; i++) {
		if (m_vec_gpu.at(i).platform == MW_VENC_PLATFORM_AMD) {
			sprintf(m_pp_cs_adapters[i], "AMD:%s",
				m_vec_gpu.at(i).gpu_name);
		} else if (m_vec_gpu.at(i).platform == MW_VENC_PLATFORM_INTEL) {
			sprintf(m_pp_cs_adapters[i], "Intel:%s",
				m_vec_gpu.at(i).gpu_name);
		} else if (m_vec_gpu.at(i).platform ==
			   MW_VENC_PLATFORM_NVIDIA) {
			sprintf(m_pp_cs_adapters[i], "Nvidia:%s",
				m_vec_gpu.at(i).gpu_name);
		}
	}
}

mw_venc_code_type_t
CMWIMGUIWidgetEncodeParam::get_code_type_from_selected(int t_n_selected)
{
	mw_venc_code_type_t t_code_type = MW_VENC_CODE_TYPE_COUNT;

	switch (t_n_selected) {
	case 0:
		t_code_type = MW_VENC_CODE_TYPE_H264;
		break;
	case 1:
		t_code_type = MW_VENC_CODE_TYPE_H265;
		break;
	default:
		break;
	}

	return t_code_type;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_code_type(
	mw_venc_code_type_t t_code_type)
{
	int t_n_selected = 0;

	switch (t_code_type) {
	case MW_VENC_CODE_TYPE_H264:
		t_n_selected = 0;
		break;
	case MW_VENC_CODE_TYPE_H265:
		t_n_selected = 1;
		break;
	default:
		break;
	}

	return t_n_selected;
}

// const char* g_cs_target_usage[] = {"Best Speed","Balanced","Best Quality"};
mw_venc_targetusage_t
CMWIMGUIWidgetEncodeParam::get_traget_usage_from_selected(int t_n_selected)
{
	mw_venc_targetusage_t t_target_usage = MW_VENC_TARGETUSAGE_COUNT;

	switch (t_n_selected) {
	case 0:
		t_target_usage = MW_VENC_TARGETUSAGE_BEST_SPEED;
		break;
	case 1:
		t_target_usage = MW_VENC_TARGETUSAGE_BALANCED;
		break;
	case 2:
		t_target_usage = MW_VENC_TARGETUSAGE_BEST_QUALITY;
		break;
	default:
		break;
	}

	return t_target_usage;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_target_usage(
	mw_venc_targetusage_t t_target_usage)
{
	int t_n_selected = 0;

	switch (t_target_usage) {
	case MW_VENC_TARGETUSAGE_BEST_SPEED:
		t_n_selected = 0;
		break;
	case MW_VENC_TARGETUSAGE_BALANCED:
		t_n_selected = 1;
		break;
	case MW_VENC_TARGETUSAGE_BEST_QUALITY:
		t_n_selected = 2;
		break;
	default:
		break;
	}

	return t_n_selected;
}

// const char* g_cs_rate_control[] = {"CBR","VBR","CQP"};
mw_venc_rate_control_mode_t
CMWIMGUIWidgetEncodeParam::get_rate_control_mode_from_selected(int t_n_selected)
{
	mw_venc_rate_control_mode_t t_rate_control_mode =
		MW_VENC_RATECONTROL_COUNT;

	switch (t_n_selected) {
	case 0:
		t_rate_control_mode = MW_VENC_RATECONTROL_CBR;
		break;
	case 1:
		t_rate_control_mode = MW_VENC_RATECONTROL_VBR;
		break;
	case 2:
		t_rate_control_mode = MW_VENC_RATECONTROL_CQP;
		break;
	default:
		break;
	}

	return t_rate_control_mode;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_rate_control_mode(
	mw_venc_rate_control_mode_t t_rate_control_mode)
{
	int t_n_rate_control_mode = 0;

	switch (t_rate_control_mode) {
	case MW_VENC_RATECONTROL_CBR:
		t_n_rate_control_mode = 0;
		break;
	case MW_VENC_RATECONTROL_VBR:
		t_n_rate_control_mode = 1;
		break;
	case MW_VENC_RATECONTROL_CQP:
		t_n_rate_control_mode = 2;
		break;
	default:
		break;
	}

	return t_n_rate_control_mode;
}

// const char* g_cs_platforms[] = { "AMD","INTEL","NVIDIA" };
mw_venc_gpu_info_t
CMWIMGUIWidgetEncodeParam::get_adapter_from_selected(int t_n_selected)
{
	mw_venc_gpu_info_t t_gpu;
	t_gpu.platform = MW_VENC_PLATFORM_COUNT;

	if (t_n_selected < m_vec_gpu.size())
		t_gpu = m_vec_gpu.at(t_n_selected);

	return t_gpu;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_adapter(
	mw_venc_gpu_info_t t_gpu)
{
	int t_n_platform = 0;

	for (int i = 0; i < m_vec_gpu.size(); i++) {
		if (m_vec_gpu.at(i).platform == t_gpu.platform &&
		    strcmp(m_vec_gpu.at(i).gpu_name, t_gpu.gpu_name) == 0) {
			t_n_platform = i;
			break;
		}
	}

	return t_n_platform;
}

// const char* g_cs_amd_mem[] = { "DX","CPU" };
mw_venc_amd_mem_type_t
CMWIMGUIWidgetEncodeParam::get_amd_mem_type_from_selected(int t_n_selected)
{
	mw_venc_amd_mem_type_t t_amd_mem_type = MW_VENC_AMD_MEM_COUNT;

	switch (t_n_selected) {
	case 0:
		t_amd_mem_type = MW_VENC_AMD_MEM_AUTO;
	case 1:
		t_amd_mem_type = MW_VENC_AMD_MEM_DX11;
		break;
	case 2:
		t_amd_mem_type = MW_VENC_AMD_MEM_DX9;
		break;
	case 3:
		t_amd_mem_type = MW_VENC_AMD_MEM_CPU;
		break;
	default:
		break;
	}

	return t_amd_mem_type;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_amd_mem_type(
	mw_venc_amd_mem_type_t t_amd_mem_type)
{
	int t_n_amd_mem_type = 0;

	switch (t_amd_mem_type) {
	case MW_VENC_AMD_MEM_AUTO:
		t_n_amd_mem_type = 0;
		break;
	case MW_VENC_AMD_MEM_DX11:
		t_n_amd_mem_type = 1;
		break;
	case MW_VENC_AMD_MEM_DX9:
		t_n_amd_mem_type = 2;
		break;
	case MW_VENC_AMD_MEM_CPU:
		t_n_amd_mem_type = 3;
		break;
	default:
		break;
	}

	return t_n_amd_mem_type;
}

// const char* g_cs_h264_profile[] = { "H264 Baseline","H264 Main","H264 High" };
// const char* g_cs_h265_profile[] = { "H265 Main" };
mw_venc_profile_t
CMWIMGUIWidgetEncodeParam::get_profile_from_selected(int t_n_selected,
						     bool t_b_h264)
{
	mw_venc_profile_t t_profile = MW_VENC_PROFILE_COUNT;

	if (t_b_h264) {
		switch (t_n_selected) {
		case 0:
			t_profile = MW_VENC_PROFILE_H264_BASELINE;
			break;
		case 1:
			t_profile = MW_VENC_PROFILE_H264_MAIN;
			break;
		case 2:
			t_profile = MW_VENC_PROFILE_H264_HIGH;
			break;
		default:
			break;
		}
	} else
		t_profile = MW_VENC_PROFILE_H265_MAIN;

	return t_profile;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_profile(
	mw_venc_profile_t t_profile)
{
	int t_n_profile = 0;

	switch (t_profile) {
	case MW_VENC_PROFILE_H264_BASELINE:
		t_n_profile = 0;
		break;
	case MW_VENC_PROFILE_H264_MAIN:
		t_n_profile = 1;
		break;
	case MW_VENC_PROFILE_H264_HIGH:
		t_n_profile = 2;
		break;
	case MW_VENC_PROFILE_H265_MAIN:
		t_n_profile = 0;
		break;
	default:
		break;
	}

	return t_n_profile;
}

// const char* g_cs_level[] = { "level 2_1","level 3_1","level 4_1","level 5_1","level 5_2","level 6_1","level 6_2" };
mw_venc_level_t
CMWIMGUIWidgetEncodeParam::get_level_from_selected(int t_n_selected)
{
	mw_venc_level_t t_level = MW_VENC_LEVEL_COUNT;

	switch (t_n_selected) {
	case 0:
		t_level = MW_VENC_LEVEL_2_1;
		break;
	case 1:
		t_level = MW_VENC_LEVEL_3_1;
		break;
	case 2:
		t_level = MW_VENC_LEVEL_4_1;
		break;
	case 3:
		t_level = MW_VENC_LEVEL_5_1;
		break;
	case 4:
		t_level = MW_VENC_LEVEL_5_2;
		break;
	case 5:
		t_level = MW_VENC_LEVEL_6_1;
		break;
	case 6:
		t_level = MW_VENC_LEVEL_6_2;
		break;
	default:
		break;
	}

	return t_level;
}

int CMWIMGUIWidgetEncodeParam::get_selected_from_level(mw_venc_level_t t_level)
{
	int t_n_level = 3;

	switch (t_level) {
	case MW_VENC_LEVEL_2_1:
		t_n_level = 0;
		break;
	case MW_VENC_LEVEL_3_1:
		t_n_level = 1;
		break;
	case MW_VENC_LEVEL_4_1:
		t_n_level = 2;
		break;
	case MW_VENC_LEVEL_5_1:
		t_n_level = 3;
		break;
	case MW_VENC_LEVEL_5_2:
		t_n_level = 4;
		break;
	case MW_VENC_LEVEL_6_1:
		t_n_level = 5;
		break;
	case MW_VENC_LEVEL_6_2:
		t_n_level = 6;
		break;
	default:
		break;
	}

	return t_n_level;
}

unsigned char *
CMWIMGUIWidgetEncodeParam::get_fourcc_from_vf_fourcc(mw_venc_fourcc_t t_fourcc)
{
	unsigned char *t_cs_fourcc = (unsigned char *)g_cs_fourcc[0];

	switch (t_fourcc) {
	case MW_VENC_FOURCC_NV12:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[0];
		break;
	case MW_VENC_FOURCC_NV21:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[1];
		break;
	case MW_VENC_FOURCC_YV12:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[2];
		break;
	case MW_VENC_FOURCC_I420:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[3];
		break;
	case MW_VENC_FOURCC_YUY2:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[4];
		break;
	case MW_VENC_FOURCC_P010:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[5];
		break;
	case MW_VENC_FOURCC_BGRA:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[6];
		break;
	case MW_VENC_FOURCC_RGBA:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[7];
		break;
	case MW_VENC_FOURCC_ARGB:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[8];
		break;
	case MW_VENC_FOURCC_ABGR:
		t_cs_fourcc = (unsigned char *)g_cs_fourcc[9];
		break;
	default:
		break;
	}

	return t_cs_fourcc;
}

bool CMWIMGUIWidgetEncodeParam::check_param_change()
{
	bool t_b_change = false;

	int t_n_adapter = get_selected_from_adapter(m_venc_gpu);
	if (t_n_adapter != m_n_adapter_selected)
		t_b_change = true;

	if (m_venc_platform == MW_VENC_PLATFORM_AMD) {
		int t_n_amd_mem = get_selected_from_amd_mem_type(
			m_venc_param.amd_mem_reserved);
		if (t_n_amd_mem != m_n_amd_mem_selected)
			t_b_change = true;
	}

	int t_n_code_type = get_selected_from_code_type(m_venc_param.code_type);
	if (t_n_code_type != m_n_encode_type)
		t_b_change = true;
	else {
		int t_n_profile =
			get_selected_from_profile(m_venc_param.profile);
		if (m_venc_param.code_type == MW_VENC_CODE_TYPE_H264) {
			if (t_n_profile != m_n_profile_h264_selected)
				t_b_change = true;
		} else {
			if (t_n_profile != m_n_profile_h265_selected)
				t_b_change = true;
		}
	}

	if (m_n_gop_size != m_venc_param.gop_pic_size)
		t_b_change = true;

	if (m_n_gop_ref_size != m_venc_param.gop_ref_size)
		t_b_change = true;

	int t_n_level = get_selected_from_level(m_venc_param.level);
	if (t_n_level != m_n_level_selected)
		t_b_change = true;

	int t_n_rate_control_mode = get_selected_from_rate_control_mode(
		m_venc_param.rate_control.mode);
	if (t_n_rate_control_mode != m_n_rate_control_selected)
		t_b_change = true;
	else {
		if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CBR) {
			if (m_venc_param.rate_control.target_bitrate !=
			    m_n_target_bitrate)
				t_b_change = true;
		} else if (m_venc_param.rate_control.mode ==
			   MW_VENC_RATECONTROL_VBR) {
			if (m_venc_param.rate_control.target_bitrate !=
				    m_n_target_bitrate ||
			    m_venc_param.rate_control.max_bitrate !=
				    m_n_max_bitrate)
				t_b_change = true;
		} else if (m_venc_param.rate_control.mode ==
			   MW_VENC_RATECONTROL_CQP) {
			if (m_venc_param.rate_control.qpi != m_n_i ||
			    m_venc_param.rate_control.qpb != m_n_b ||
			    m_venc_param.rate_control.qpp != m_n_p)
				t_b_change = true;
		}
	}

	if (m_venc_param.slice_num != m_n_slice_num)
		t_b_change = true;

	int t_n_target_usage =
		get_selected_from_target_usage(m_venc_param.targetusage);
	if (t_n_target_usage != m_n_target_usage_selected)
		t_b_change = true;

	return t_b_change;
}

void CMWIMGUIWidgetEncodeParam::set_venc_param()
{
	m_venc_gpu = get_adapter_from_selected(m_n_adapter_selected);
	m_venc_platform = m_venc_gpu.platform;
	if (m_venc_platform == MW_VENC_PLATFORM_AMD)
		m_venc_param.amd_mem_reserved =
			get_amd_mem_type_from_selected(m_n_amd_mem_selected);

	m_venc_param.code_type = get_code_type_from_selected(m_n_encode_type);
	bool t_b_h264 =
		m_venc_param.code_type == MW_VENC_CODE_TYPE_H264 ? true : false;
	if (t_b_h264)
		m_venc_param.profile = get_profile_from_selected(
			m_n_profile_h264_selected, true);
	else
		m_venc_param.profile = get_profile_from_selected(
			m_n_profile_h265_selected, false);

	m_venc_param.fourcc = m_vf_fourcc;
	m_venc_param.fps.den = m_n_fps_den;
	m_venc_param.fps.num = m_n_fps_num;
	m_venc_param.gop_pic_size = m_n_gop_size;
	m_venc_param.gop_ref_size = m_n_gop_ref_size;
	m_venc_param.width = m_n_width;
	m_venc_param.height = m_n_height;
	m_venc_param.level = get_level_from_selected(m_n_level_selected);

	m_venc_param.rate_control.mode =
		get_rate_control_mode_from_selected(m_n_rate_control_selected);
	if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CBR)
		m_venc_param.rate_control.target_bitrate = m_n_target_bitrate;
	else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_VBR) {
		m_venc_param.rate_control.target_bitrate = m_n_target_bitrate;
		m_venc_param.rate_control.max_bitrate = m_n_max_bitrate;
	} else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CQP) {
		m_venc_param.rate_control.qpi = m_n_i;
		m_venc_param.rate_control.qpb = m_n_b;
		m_venc_param.rate_control.qpp = m_n_p;
	}

	m_venc_param.slice_num = m_n_slice_num;

	m_venc_param.targetusage =
		get_traget_usage_from_selected(m_n_target_usage_selected);
}

void CMWIMGUIWidgetEncodeParam::set_ui_param()
{
	m_n_adapter_selected = get_selected_from_adapter(m_venc_gpu);
	if (m_venc_gpu.platform == MW_VENC_PLATFORM_AMD)
		m_n_amd_mem_selected = get_selected_from_amd_mem_type(
			m_venc_param.amd_mem_reserved);

	m_n_encode_type = get_selected_from_code_type(m_venc_param.code_type);
	bool t_b_h264 =
		m_venc_param.code_type == MW_VENC_CODE_TYPE_H264 ? true : false;
	if (t_b_h264)
		m_n_profile_h264_selected =
			get_selected_from_profile(m_venc_param.profile);
	else
		m_n_profile_h265_selected =
			get_selected_from_profile(m_venc_param.profile);

	m_vf_fourcc = m_venc_param.fourcc;
	m_n_fps_den = m_venc_param.fps.den;
	m_n_fps_num = m_venc_param.fps.num;
	m_n_gop_size = m_venc_param.gop_pic_size;
	m_n_gop_ref_size = m_venc_param.gop_ref_size;
	m_n_width = m_venc_param.width;
	m_n_height = m_venc_param.height;
	m_n_level_selected = get_selected_from_level(m_venc_param.level);

	m_n_rate_control_selected = get_selected_from_rate_control_mode(
		m_venc_param.rate_control.mode);
	if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CBR)
		m_n_target_bitrate = m_venc_param.rate_control.target_bitrate;
	else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_VBR) {
		m_n_target_bitrate = m_venc_param.rate_control.target_bitrate;
		m_n_max_bitrate = m_venc_param.rate_control.max_bitrate;
	} else if (m_venc_param.rate_control.mode == MW_VENC_RATECONTROL_CQP) {
		m_n_i = m_venc_param.rate_control.qpi;
		m_n_p = m_venc_param.rate_control.qpp;
		m_n_b = m_venc_param.rate_control.qpb;
	}

	m_n_slice_num = m_venc_param.slice_num;

	m_n_target_usage_selected =
		get_selected_from_target_usage(m_venc_param.targetusage);
}
