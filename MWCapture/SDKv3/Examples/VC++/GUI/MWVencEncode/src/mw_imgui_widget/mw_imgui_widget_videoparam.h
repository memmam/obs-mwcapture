/************************************************************************************************/
// mw_imgui_widget_videoparam.h : interface of the CMWIMGUIWidgetVideoParam class

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

#ifndef MW_IMGUI_WIDGET_VIDEOPARAM
#define MW_IMGUI_WIDGET_VIDEOPARAM

#include "mw_imgui_widget.h"

#include "stdlib.h"
#include "LibMWCapture/MWCapture.h"

#include "vector"
using namespace std;

struct cmw_ui_video_capture_param_s
{
	int					m_n_width;
	int					m_n_height;
	int					m_n_frameduration;
	uint32_t			m_u32_color;
};

typedef void(*CALLBACK_PARAM_CHANGE)(cmw_ui_video_capture_param_s t_param,void* t_p_param);

class CMWIMGUIWidgetVideoParam:public CMWIMGUIWidget
{
public:
	CMWIMGUIWidgetVideoParam(CMWIMGUIUIManager * t_p_manager);
	~CMWIMGUIWidgetVideoParam();

public:
	void update_widget();
	void reset_ui();

	cmw_ui_video_capture_param_s get_ui_video_capture_param();

	void set_param_change_callback(CALLBACK_PARAM_CHANGE t_callback,void* t_p_param);

	void set_supported_resolution(vector<MWSIZE> t_vec_resolution,MW_FAMILY_ID t_family_id);

	void set_supported_frameduration(vector<uint64_t> t_vec_duration,MW_FAMILY_ID t_family_id);

	void set_supported_colorformat(vector<uint32_t> t_vec_fourcc,MW_FAMILY_ID t_family_id);

protected:
	void trans_fourcc(char* t_cs_fourcc,uint32_t t_u32_fourcc);

	int get_index_resolution(MWSIZE t_size);
	int get_index_resolution_eco_bgra(MWSIZE t_size);
	int get_index_frameduration(uint64_t t_u64_duration);
	int get_index_frameduration_4krgba(uint64_t t_u64_duration);
	int get_index_color(uint32_t t_u32_color);
	int	get_index_color_lack_usb(uint32_t t_u32_color);

protected:
	int								m_n_width;
	int								m_n_height;
	int								m_n_selected_resolution;
	int								m_n_selected_resolution_eco_bgra;

	int								m_n_selected_color;
	int								m_n_selected_color_lack_bgra;
	uint32_t						m_u32_color;

	int								m_n_frameduration;
	int								m_n_selected_frameduration;
	int								m_n_selected_4krgba_frameduration;

	int								m_n_color_num;
	int								m_n_ticks_per_second;

	cmw_ui_video_capture_param_s	m_video_capture_param;

	CALLBACK_PARAM_CHANGE			m_callback_change;
	void*							m_p_caller;

	char**							m_pp_cs_duration;
	int								m_n_num_duration;
	int								m_n_len_duration;
	int								m_n_count_duration;
	vector<uint64_t>				m_vec_duration;

	char**							m_pp_cs_4krgba_duration;
	int								m_n_num_4krgba_duration;
	int								m_n_len_4krgba_duration;
	int								m_n_count_4krgba_duration;
	vector<uint64_t>				m_vec_4krgba_duration;

	char**							m_pp_cs_resolution;
	int								m_n_num_resolution;
	int								m_n_len_resolution;
	int								m_n_count_resolution;
	vector<MWSIZE>					m_vec_resolution;

	char**							m_pp_cs_eco_bgra_resolution;
	int								m_n_num_eco_bgra_resolution;
	int								m_n_len_eco_bgra_resolution;
	int								m_n_count_eco_bgra_resolution;
	vector<MWSIZE>					m_vec_eco_bgra_resolution;

	char**							m_pp_cs_format;
	int								m_n_num_format;
	int								m_n_len_format;
	int								m_n_count_format;
	vector<uint32_t>				m_vec_format;

	char**							m_pp_cs_format_lack_bgra;
	int								m_n_num_format_lack_bgra;
	int								m_n_len_format_lack_bgra;
	int								m_n_count_format_lack_bgra;
	vector<uint32_t>				m_vec_format_lack_bgra;

	MW_FAMILY_ID					m_device_family;
};

#endif