#include "mw_cc_layer.h"

CMWCCLayer::CMWCCLayer(CMWCCPlayerUIManager *t_p_manager)
	: CMWIMGUIWidget(t_p_manager)
{
	m_imgui_flags = ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMouseInputs;

	m_b_open = true;
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "CC Layer");
	m_f_bg_alpha = 0.0;
	m_imvec2_pos.x = 10;
	m_imvec2_pos.y = 10;

	m_imvec2_size.x = 600;
	m_imvec2_size.y = 110;
}

CMWCCLayer::~CMWCCLayer() {}

void CMWCCLayer::update_widget()
{
	ImGuiIO &io = ImGui::GetIO();
	ImVec2 t_imvec2_pos;
	t_imvec2_pos.x = 0;
	t_imvec2_pos.y = 18;

	m_imvec2_size.x = io.DisplaySize.x;
	m_imvec2_size.y = io.DisplaySize.y - 18;

	ImGui::SetNextWindowPos(t_imvec2_pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(m_imvec2_size, ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(m_f_bg_alpha);
	ImGui::Begin(m_cs_name, &m_b_open, m_imgui_flags);

	// draw cc
	ImDrawList *t_p_list = ImGui::GetWindowDrawList();
	t_p_list->AddImageQuad((void *)(intptr_t)m_ui_texture_id,
			       ImVec2(0, m_imvec2_size.y),
			       ImVec2(m_imvec2_size.x, m_imvec2_size.y),
			       ImVec2(m_imvec2_size.x, 18), ImVec2(0, 18));

	ImGui::End();
}

void CMWCCLayer::set_texture_id(unsigned int t_ui_id)
{
	m_ui_texture_id = t_ui_id;
}

char *CMWCCLayer::get_name()
{
	return m_cs_name;
}

void CMWCCLayer::set_name(char *t_cs_name)
{
	memset(m_cs_name, 0, 128);
	sprintf(m_cs_name, "%s", t_cs_name);
}

bool CMWCCLayer::get_is_open()
{
	return m_b_open;
}

void CMWCCLayer::set_is_open(bool t_b_open)
{
	m_b_open = t_b_open;
}

float CMWCCLayer::get_bg_alpha()
{
	return m_f_bg_alpha;
}

void CMWCCLayer::set_bg_alpha(float t_f_alpha)
{
	m_f_bg_alpha = t_f_alpha;
}

ImVec2 CMWCCLayer::get_pos()
{
	return m_imvec2_pos;
}

void CMWCCLayer::set_pos(ImVec2 t_pos)
{
	m_imvec2_pos = t_pos;
}
