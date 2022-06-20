/************************************************************************************************/
// mw_hdr_value.h : interface of the CMWHDRParam class

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

#ifndef MW_HDR_VALUE_H
#define MW_HDR_VALUE_H

#include "mw_imgui_widget.h"

struct cmw_hdr_param_s {
	int32_t m_i32_luminance;
};

typedef void (*CALLBACK_PARAM_CHANGE)(cmw_hdr_param_s t_param, void *t_p_param);

class CMWHDRParam : public CMWIMGUIWidget {
public:
	CMWHDRParam(CMWHDRCaptureUIManager *t_p_manager);
	~CMWHDRParam();

public:
	void update_widget();
	cmw_hdr_param_s get_ui_video_capture_param();

	void set_param_change_callback(CALLBACK_PARAM_CHANGE t_callback,
				       void *t_p_param);

protected:
	cmw_hdr_param_s m_video_capture_param;
	cmw_hdr_param_s m_video_capture_param_back;

	CALLBACK_PARAM_CHANGE m_callback_change;
	void *m_p_caller;
};

#endif