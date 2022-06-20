#include "mw_hdr_overlay_text.h"

CMWHDROverlatText::CMWHDROverlatText(CMWHDRCaptureUIManager* t_p_manager):
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

CMWHDROverlatText::~CMWHDROverlatText()
{

}

void CMWHDROverlatText::update_widget()
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

char* CMWHDROverlatText::get_name()
{
	return m_cs_name;
}

void CMWHDROverlatText::set_name(char* t_cs_name)
{
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "%s", t_cs_name);
}

char* CMWHDROverlatText::get_text()
{
	return m_cs_text;
}

void CMWHDROverlatText::set_text(char* t_cs_text)
{
	memset(m_cs_text, 0, 1024);
	sprintf(m_cs_text, "%s", t_cs_text);
}

bool CMWHDROverlatText::get_is_open()
{
	return m_b_open;
}

void CMWHDROverlatText::set_is_open(bool t_b_open)
{
	m_b_open = t_b_open;
}

float CMWHDROverlatText::get_bg_alpha()
{
	return m_f_bg_alpha;
}

void CMWHDROverlatText::set_bg_alpha(float t_f_alpha)
{
	m_f_bg_alpha = t_f_alpha;
}

ImVec2 CMWHDROverlatText::get_pos()
{
	return m_imvec2_pos;
}

void CMWHDROverlatText::set_pos(ImVec2 t_pos)
{
	m_imvec2_pos = t_pos;
}
