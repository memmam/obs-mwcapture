/************************************************************************************************/
// mw_cc_player_ui_manager.h : interface of the CMWCCPlayerUIManager class

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

#ifndef MW_CC_PLAYER_UI_MANAGER_H
#define MW_CC_PLAYER_UI_MANAGER_H

#include "../mw_imgui_widget/mw_cc_player_menubar.h"
#include "../mw_imgui_widget/mw_cc_setting.h"
#include "../mw_imgui_widget/mw_encode_adapter.h"
#include "../mw_imgui_widget/mw_cc_overlay_text.h"
#include "../mw_imgui_widget/mw_cc_layer.h"

class CMWCCPlayerUIManager{
public:
	CMWCCPlayerUIManager();
	~CMWCCPlayerUIManager();

public:
	void add_widget(CMWIMGUIWidget* t_p_wid);
	void update_ui();

private:
	vector<CMWIMGUIWidget*>		m_vec_widget;
};

#endif
