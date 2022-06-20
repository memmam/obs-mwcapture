/************************************************************************************************/
// mw_hdr_value.cpp : implementation of the CMWHDRParam class

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

#include "mw_hdr_value.h"

CMWHDRParam::CMWHDRParam(CMWHDRCaptureUIManager *t_p_manager)
	: CMWIMGUIWidget(t_p_manager)
{
	m_video_capture_param.m_i32_luminance = 40;

	m_video_capture_param_back.m_i32_luminance = 40;

	m_callback_change = NULL;
	m_p_caller = NULL;
}

CMWHDRParam::~CMWHDRParam() {}

void CMWHDRParam::update_widget()
{
	ImGui::SliderInt("Luminance", &m_video_capture_param.m_i32_luminance, 0,
			 100);

	if (m_video_capture_param.m_i32_luminance !=
	    m_video_capture_param_back.m_i32_luminance) {
		if (m_callback_change)
			m_callback_change(m_video_capture_param, m_p_caller);

		m_video_capture_param_back.m_i32_luminance =
			m_video_capture_param.m_i32_luminance;
	}
}

cmw_hdr_param_s CMWHDRParam::get_ui_video_capture_param()
{
	return m_video_capture_param;
}

void CMWHDRParam::set_param_change_callback(CALLBACK_PARAM_CHANGE t_callback,
					    void *t_p_param)
{
	m_callback_change = t_callback;
	m_p_caller = t_p_param;
}
