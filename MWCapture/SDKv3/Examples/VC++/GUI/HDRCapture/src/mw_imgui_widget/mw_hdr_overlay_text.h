/************************************************************************************************/
// mw_imgui_widget_overlay_text.h : interface of the CMWIMGUIWidgetOverlayText class

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
#ifndef MW_HDR_OVERLAY_TEXT_H
#define MW_HDR_OVERLAY_TEXT_H

#include "mw_imgui_widget.h"
#include "string.h"
using namespace std;

class CMWHDROverlatText:public CMWIMGUIWidget
{
public:
	CMWHDROverlatText(CMWHDRCaptureUIManager* t_p_manager);
	~CMWHDROverlatText();

public:
	void update_widget();

	char* get_name();
	void set_name(char* t_cs_name);
	char* get_text();
	void set_text(char* t_cs_text);
	bool get_is_open();
	void set_is_open(bool t_b_open);
	float get_bg_alpha();
	void set_bg_alpha(float t_f_alpha);
	ImVec2 get_pos();
	void set_pos(ImVec2 t_pos);

private:
	ImGuiWindowFlags						m_imgui_flags;
	char									m_cs_name[128];
	bool									m_b_open;
	float									m_f_bg_alpha;
	ImVec2									m_imvec2_pos;
	ImVec2									m_imvec2_size;

	char									m_cs_text[1024];
};

#endif