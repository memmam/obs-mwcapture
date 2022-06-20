/************************************************************************************************/
// mw_cc_setting.h : interface of the CMWCCSetting class

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

#ifndef MW_CC_SETTING_H
#define MW_CC_SETTING_H

#include "mw_imgui_widget.h"

typedef struct mw_cc_setting_param_s{
	bool	m_b_cc708;
	bool	m_b_cc608;
	int		m_n_cc608_select;
	int		m_n_font_size;
	float	m_back_color[4];
	float	m_fore_color[4];
	int		m_n_cc_width;
	int		m_n_cc_height;
}mw_cc_setting_param_t;

class CMWCCSetting:public CMWIMGUIWidget
{
public:
	CMWCCSetting(CMWCCPlayerUIManager* t_p_manager);
	~CMWCCSetting();

public:
	void update_widget();
	
	bool update_cc_param(mw_cc_setting_param_t* t_p_cc_param);
	mw_cc_setting_param_t get_cc_param();

protected:
	bool m_b_checked_cc708;
	bool m_b_checked_cc608;
	int	 m_n_cc608_select;

	int m_n_font_size;
	int m_n_font_size_min;
	int m_n_font_size_max;

	float m_back_color[4];
	float m_fore_color[4];

	int m_n_cc_width;
	int m_n_cc_height;

	mw_cc_setting_param_t	m_cc_param;
	bool					m_b_change;
};

#endif
