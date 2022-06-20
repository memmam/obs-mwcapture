/************************************************************************************************/
// mw_imgui_widget_overlay_text.cpp : implementation of the CMWIMGUIWidgetOverlayText class

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

#include "mw_imgui_widget_overlay_text.h"
#include "stdio.h"

CMWIMGUIWidgetOverlayText::CMWIMGUIWidgetOverlayText(CMWIMGUIUIManager* t_p_manager):
	CMWIMGUIWidget(t_p_manager)
{
	m_imgui_flags = ImGuiWindowFlags_NoMove|
					ImGuiWindowFlags_NoDecoration|
					ImGuiWindowFlags_AlwaysAutoResize|
					ImGuiWindowFlags_NoSavedSettings|
					ImGuiWindowFlags_NoFocusOnAppearing|
					ImGuiWindowFlags_NoNav;

	m_b_open = true;
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "Overlay Text");
	m_f_bg_alpha = 0.35;
	m_imvec2_pos.x = 10;
	m_imvec2_pos.y = 10;

	m_imvec2_size.x = 600;
	m_imvec2_size.y = 100;

	memset(m_cs_text, 0, 1024);
	sprintf(m_cs_text, "text");
}

CMWIMGUIWidgetOverlayText::~CMWIMGUIWidgetOverlayText()
{

}

void CMWIMGUIWidgetOverlayText::update_widget()
{
	ImGuiIO& io = ImGui::GetIO();
	ImVec2 t_imvec2_pos;
	t_imvec2_pos.x = io.DisplaySize.x - m_imvec2_size.x - 10;
	t_imvec2_pos.y = io.DisplaySize.y - m_imvec2_size.y - 10;
	ImGui::SetNextWindowPos(t_imvec2_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(m_imvec2_size, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(m_f_bg_alpha);
	ImGui::Begin(m_cs_name, &m_b_open, m_imgui_flags);

	ImGui::Text("%s", m_cs_text);

	ImGui::End();
}

char* CMWIMGUIWidgetOverlayText::get_name()
{
	return m_cs_name;
}

void CMWIMGUIWidgetOverlayText::set_name(char* t_cs_name)
{
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "%s", t_cs_name);
}

char* CMWIMGUIWidgetOverlayText::get_text()
{
	return m_cs_text;
}

void CMWIMGUIWidgetOverlayText::set_text(char* t_cs_text)
{
	memset(m_cs_text, 0, 1024);
	sprintf(m_cs_text, "%s", t_cs_text);
}

bool CMWIMGUIWidgetOverlayText::get_is_open()
{
	return m_b_open;
}

void CMWIMGUIWidgetOverlayText::set_is_open(bool t_b_open)
{
	m_b_open = t_b_open;
}

float CMWIMGUIWidgetOverlayText::get_bg_alpha()
{
	return m_f_bg_alpha;
}

void CMWIMGUIWidgetOverlayText::set_bg_alpha(float t_f_alpha)
{
	m_f_bg_alpha = t_f_alpha;
}

ImVec2 CMWIMGUIWidgetOverlayText::get_pos()
{
	return m_imvec2_pos;
}

void CMWIMGUIWidgetOverlayText::set_pos(ImVec2 t_pos)
{
	m_imvec2_pos = t_pos;
}
