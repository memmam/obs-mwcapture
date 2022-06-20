/************************************************************************************************/
// mw_imgui_widget_encodeparam.h : interface of the CMWIMGUIWidgetEncodeParam class

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

#ifndef MW_IMGUI_WIDGET_ENCODE_PARAM_H
#define MW_IMGUI_WIDGET_ENCODE_PARAM_H

#include "mw_imgui_widget.h"
#include "LibMWVenc/mw_venc.h"
#include "vector"
using namespace std;

struct cmw_video_base_s {
	int m_n_width;
	int m_n_height;
	mw_venc_fourcc_t m_vf_fourcc;
	int m_n_fps_den;
	int m_n_fps_num;
};

typedef void (*CALLBACK_ENABLE_ENCODE)(bool t_b_enable, void *t_p_param);

class CMWIMGUIWidgetEncodeParam : public CMWIMGUIWidget {
public:
	CMWIMGUIWidgetEncodeParam(CMWIMGUIUIManager *t_p_manager);
	~CMWIMGUIWidgetEncodeParam();

public:
	void update_widget();

	void reset_ui();

	void set_enable_encode_callback(CALLBACK_ENABLE_ENCODE t_callback,
					void *t_p_param);

	mw_venc_param_t get_encode_param();
	mw_venc_gpu_info_t get_encode_adapter();
	int get_encoder_index();
	void set_video_base(cmw_video_base_s t_base);

	void init_support_platform(vector<mw_venc_platform_t> t_vec_platform,
				   vector<mw_venc_gpu_info> t_vec_gpu,
				   vector<int> t_vec_index);

protected:
	mw_venc_code_type_t get_code_type_from_selected(int t_n_selected);
	int get_selected_from_code_type(mw_venc_code_type_t t_code_type);
	mw_venc_targetusage_t get_traget_usage_from_selected(int t_n_selected);
	int
	get_selected_from_target_usage(mw_venc_targetusage_t t_target_usage);
	mw_venc_rate_control_mode_t
	get_rate_control_mode_from_selected(int t_n_selected);
	int get_selected_from_rate_control_mode(
		mw_venc_rate_control_mode_t t_rate_control_mode);
	mw_venc_gpu_info_t get_adapter_from_selected(int t_n_selected);
	int get_selected_from_adapter(mw_venc_gpu_info_t t_gpu);
	mw_venc_amd_mem_type_t get_amd_mem_type_from_selected(int t_n_selected);
	int
	get_selected_from_amd_mem_type(mw_venc_amd_mem_type_t t_amd_mem_type);
	mw_venc_profile_t get_profile_from_selected(int t_n_selected,
						    bool t_b_h264);
	int get_selected_from_profile(mw_venc_profile_t t_profile);
	mw_venc_level_t get_level_from_selected(int t_n_selected);
	int get_selected_from_level(mw_venc_level_t t_level);

	unsigned char *get_fourcc_from_vf_fourcc(mw_venc_fourcc_t t_fourcc);

	bool check_param_change();
	void set_venc_param();
	void set_ui_param();

private:
	int m_n_adapter_selected;
	int m_n_amd_mem_selected;
	int m_n_encode_type;
	int m_n_target_usage_selected;
	int m_n_rate_control_selected;
	int m_n_target_bitrate;
	int m_n_max_bitrate;

	int m_n_i;
	int m_n_b;
	int m_n_p;

	int m_n_slice_num; // slice num,default 1

	int m_n_gop_size; // gop size, default 60

	int m_n_gop_ref_size; //gop ref size,default 0,if 1->no bframe

	int m_n_profile_h264_selected; // h264 profile selected
	int m_n_profile_h265_selected; // h265 profile selected

	int m_n_level_selected; // level

	CALLBACK_ENABLE_ENCODE m_callback_enable_encode;
	void *m_p_caller;

	mw_venc_param_t m_venc_param;
	mw_venc_platform m_venc_platform;
	mw_venc_gpu_info_t m_venc_gpu;
	int m_n_gpu_index;
	int m_n_width;
	int m_n_height;
	mw_venc_fourcc_t m_vf_fourcc;
	int m_n_fps_den;
	int m_n_fps_num;

	char **m_pp_cs_adapters;
	int m_n_num_platform;
	int m_n_len_platform;
	int m_n_count_adapters;
	vector<mw_venc_platform_t> m_vec_platform;
	vector<mw_venc_gpu_info_t> m_vec_gpu;
	vector<int> m_vec_index;
};

#endif
