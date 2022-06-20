/************************************************************************************************/
// mw_cc_setting.cpp : implementation of the CMWCCSetting class

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

#include "mw_cc_setting.h"

CMWCCSetting::CMWCCSetting(CMWCCPlayerUIManager* t_p_manager):
	CMWIMGUIWidget(t_p_manager)
{
	m_b_checked_cc708 = true;
	m_b_checked_cc608 = true;
	m_n_cc608_select = 0;

	m_n_font_size = 100;
	m_n_font_size_min = 50;
	m_n_font_size_max = 100;

	m_back_color[0]=0;
	m_back_color[1]=0;
	m_back_color[2]=0;
	m_back_color[3]=1.0;

	m_fore_color[0]=1.0;
	m_fore_color[1]=1.0;
	m_fore_color[2]=1.0;
	m_fore_color[3]=1.0;

	m_n_cc_width = 720;
	m_n_cc_height = 480;

	m_cc_param.m_b_cc708 = true;
	m_cc_param.m_b_cc608 = true;
	m_cc_param.m_n_cc608_select = 0;
	m_cc_param.m_n_font_size = 100;

	m_cc_param.m_back_color[0] = m_back_color[0];
	m_cc_param.m_back_color[1] = m_back_color[1];
	m_cc_param.m_back_color[2] = m_back_color[2];
	m_cc_param.m_back_color[3] = m_back_color[3];

	m_cc_param.m_fore_color[0] = m_fore_color[0];
	m_cc_param.m_fore_color[1] = m_fore_color[1];
	m_cc_param.m_fore_color[2] = m_fore_color[2];
	m_cc_param.m_fore_color[3] = m_fore_color[3];

	m_cc_param.m_n_cc_width = m_n_cc_width;
	m_cc_param.m_n_cc_height = m_n_cc_height;

	m_b_change = true;
}

CMWCCSetting::~CMWCCSetting()
{

}

void CMWCCSetting::update_widget()
{
	const char* items[] = { "All","CC1", "CC2", "CC3", "CC4"};
	static const char* item_current = items[0];   

	if(ImGui::CollapsingHeader("CC Show Setting")){
		ImGui::Checkbox("CC708",&m_b_checked_cc708);
		ImGui::Checkbox("CC608",&m_b_checked_cc608);
		if(m_b_checked_cc608){
	         // Here our selection is a single pointer stored outside the object.
			if (ImGui::BeginCombo("CC608 Show", item_current)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (item_current == items[n]);
					if (ImGui::Selectable(items[n], is_selected)){
						item_current = items[n];
						is_selected = true;
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
						m_n_cc608_select = n;
					}
				}
				ImGui::EndCombo();
			}
		}
	}
	
	if(ImGui::CollapsingHeader("Font Settings")){
		ImGui::SliderInt("Font Size",&m_n_font_size,m_n_font_size_min,m_n_font_size_max);
		ImGui::ColorEdit4("Background",m_back_color);
		ImGui::ColorEdit4("Foreground",m_fore_color);
	}

	ImGui::Separator();
	ImGui::InputInt("CC Width",&m_n_cc_width,2);
	m_n_cc_width = m_n_cc_width - m_n_cc_width%2;
	m_n_cc_width = m_n_cc_width<160?160:m_n_cc_width;
	m_n_cc_width = m_n_cc_width>1920?1920:m_n_cc_width;

	ImGui::InputInt("CC Height",&m_n_cc_height,2);
	m_n_cc_height = m_n_cc_height - m_n_cc_height%2;
	m_n_cc_height = m_n_cc_height<90?90:m_n_cc_height;
	m_n_cc_height = m_n_cc_height>1080?1080:m_n_cc_height;
}

bool 
CMWCCSetting::update_cc_param(mw_cc_setting_param_t* t_p_cc_param)
{
	m_b_change = false;

	if(m_cc_param.m_b_cc708!=m_b_checked_cc708){
		m_b_change = true;
		m_cc_param.m_b_cc708 = m_b_checked_cc708;
	}

	if(m_cc_param.m_b_cc608 != m_b_checked_cc608){
		m_b_change = true;
		m_cc_param.m_b_cc608 = m_b_checked_cc608;
	}

	if(m_cc_param.m_n_cc608_select != m_n_cc608_select){
		m_b_change = true;
		m_cc_param.m_n_cc608_select = m_n_cc608_select;
	}

	if(m_cc_param.m_n_font_size != m_n_font_size){
		m_b_change = true;
		m_cc_param.m_n_font_size = m_n_font_size;
	}

	if(m_cc_param.m_back_color[0] != m_back_color[0]||
	   m_cc_param.m_back_color[1] != m_back_color[1]||
	   m_cc_param.m_back_color[2] != m_back_color[2]||
	   m_cc_param.m_back_color[3] != m_back_color[3]){
		m_b_change = true;
		m_cc_param.m_back_color[0] = m_back_color[0];
		m_cc_param.m_back_color[1] = m_back_color[1];
		m_cc_param.m_back_color[2] = m_back_color[2];
		m_cc_param.m_back_color[3] = m_back_color[3];
	}

	if(m_cc_param.m_fore_color[0] != m_fore_color[0]||
	   m_cc_param.m_fore_color[1] != m_fore_color[1]||
	   m_cc_param.m_fore_color[2] != m_fore_color[2]||
	   m_cc_param.m_fore_color[3] != m_fore_color[3]){
		m_b_change = true;
		m_cc_param.m_fore_color[0] = m_fore_color[0];
		m_cc_param.m_fore_color[1] = m_fore_color[1];
		m_cc_param.m_fore_color[2] = m_fore_color[2];
		m_cc_param.m_fore_color[3] = m_fore_color[3];
	}

	if(m_cc_param.m_n_cc_width != m_n_cc_width){
		m_b_change = true;
		m_cc_param.m_n_cc_width = m_n_cc_width;
	}

	if(m_cc_param.m_n_cc_height != m_n_cc_height){
		m_b_change = true;
		m_cc_param.m_n_cc_height = m_n_cc_height;
	}

	if(m_b_change){
		*t_p_cc_param = m_cc_param;
		return true;
	}

	return false; 
}

mw_cc_setting_param_t CMWCCSetting::get_cc_param()
{
	return m_cc_param;
}
