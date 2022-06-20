#include "mw_cc_overlay_text.h"

CMWCCOverlayText::CMWCCOverlayText(CMWCCPlayerUIManager* t_p_manager):
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

CMWCCOverlayText::~CMWCCOverlayText()
{

}

void CMWCCOverlayText::update_widget()
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

char* CMWCCOverlayText::get_name()
{
	return m_cs_name;
}

void CMWCCOverlayText::set_name(char* t_cs_name)
{
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "%s", t_cs_name);
}

char* CMWCCOverlayText::get_text()
{
	return m_cs_text;
}

void CMWCCOverlayText::set_text(char* t_cs_text)
{
	memset(m_cs_text, 0, 1024);
	sprintf(m_cs_text, "%s", t_cs_text);
}

bool CMWCCOverlayText::get_is_open()
{
	return m_b_open;
}

void CMWCCOverlayText::set_is_open(bool t_b_open)
{
	m_b_open = t_b_open;
}

float CMWCCOverlayText::get_bg_alpha()
{
	return m_f_bg_alpha;
}

void CMWCCOverlayText::set_bg_alpha(float t_f_alpha)
{
	m_f_bg_alpha = t_f_alpha;
}

ImVec2 CMWCCOverlayText::get_pos()
{
	return m_imvec2_pos;
}

void CMWCCOverlayText::set_pos(ImVec2 t_pos)
{
	m_imvec2_pos = t_pos;
}

