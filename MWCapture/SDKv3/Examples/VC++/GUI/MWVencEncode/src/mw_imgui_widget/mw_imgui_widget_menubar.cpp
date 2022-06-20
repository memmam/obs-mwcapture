/************************************************************************************************/
// mw_imgui_widget_menubar.cpp : implementation of the CMWIMGUIWidgetMenuBar class

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

#include "mw_imgui_widget_menubar.h"

CMWIMGUIWidgetMenuBar::CMWIMGUIWidgetMenuBar(CMWIMGUIUIManager *t_p_manager)
	: CMWIMGUIWidget(t_p_manager)
{
	m_vec_menu.clear();
	m_callback_selected = NULL;
	m_p_caller = NULL;
}

CMWIMGUIWidgetMenuBar::~CMWIMGUIWidgetMenuBar()
{
	while (!m_vec_menu.empty()) {
		if (m_vec_menu.back() != NULL) {
			if (m_vec_menu.back()->m_pvec_menu_item != NULL) {
				delete m_vec_menu.back()->m_pvec_menu_item;
				m_vec_menu.back()->m_pvec_menu_item = NULL;
			}
			delete m_vec_menu.back();
		}
		m_vec_menu.pop_back();
	}
}

void CMWIMGUIWidgetMenuBar::update_widget()
{
	if (ImGui::BeginMainMenuBar()) {

		if (m_vec_menu.empty()) {
			if (ImGui::BeginMenu("menu", false)) {
				ImGui::EndMenu();
			}
		} else {
			for (int i = 0; i < m_vec_menu.size(); i++) {
				if (ImGui::BeginMenu(m_vec_menu.at(i)->m_cs_menu,
						     true)) {
					vector<cmw_menu_item_s> *t_pvec_menu_item =
						m_vec_menu.at(i)
							->m_pvec_menu_item;
					if (t_pvec_menu_item != NULL) {
						bool t_b_change = false;
						int t_n_change = 0;
						for (int j = 0;
						     j <
						     t_pvec_menu_item->size();
						     j++) {
							if (t_pvec_menu_item
								    ->at(j)
								    .m_p_sub_menu !=
							    NULL) {
								if (ImGui::BeginMenu(
									    t_pvec_menu_item
										    ->at(j)
										    .m_cs_menu_item,
									    t_pvec_menu_item
										    ->at(j)
										    .m_b_enable)) {
									t_pvec_menu_item
										->at(j)
										.m_p_sub_menu
										->update_widget();
									ImGui::EndMenu();
								} else {
									if (t_pvec_menu_item
										    ->at(j)
										    .m_b_enable) {
										t_pvec_menu_item
											->at(j)
											.m_p_sub_menu
											->reset_ui();
									}
								}
							} else {
								bool t_b_press = ImGui::MenuItem(
									t_pvec_menu_item
										->at(j)
										.m_cs_menu_item,
									NULL,
									&t_pvec_menu_item
										 ->at(j)
										 .m_b_selected,
									t_pvec_menu_item
										->at(j)
										.m_b_enable);
								if (t_b_press ==
								    true) {
									t_n_change =
										j;
									t_b_change =
										true;
								}
							}
						}
						if (t_b_change) {
							if (i == 1) {
								for (int j = 0;
								     j <
								     t_pvec_menu_item
									     ->size();
								     j++) {
									if (j !=
									    t_n_change)
										t_pvec_menu_item
											->at(j)
											.m_b_selected =
											false;
									else
										t_pvec_menu_item
											->at(j)
											.m_b_selected =
											true;
								}

								if (t_n_change !=
								    m_vec_menu
									    .at(i)
									    ->m_n_selected) {
									m_vec_menu
										.at(i)
										->m_n_selected =
										t_n_change;
									if (m_callback_selected !=
									    NULL)
										m_callback_selected(
											m_vec_menu
												.at(i)
												->m_cs_menu,
											t_pvec_menu_item
												->at(m_vec_menu
													     .at(i)
													     ->m_n_selected)
												.m_cs_menu_item,
											m_vec_menu
												.at(i)
												->m_n_selected,
											m_p_caller);
								}
							} else {
								for (int j = 0;
								     j <
								     t_pvec_menu_item
									     ->size();
								     j++)
									t_pvec_menu_item
										->at(j)
										.m_b_selected =
										false;

								m_vec_menu
									.at(i)
									->m_n_selected =
									t_n_change;
								if (m_callback_selected !=
								    NULL)
									m_callback_selected(
										m_vec_menu
											.at(i)
											->m_cs_menu,
										t_pvec_menu_item
											->at(m_vec_menu
												     .at(i)
												     ->m_n_selected)
											.m_cs_menu_item,
										m_vec_menu
											.at(i)
											->m_n_selected,
										m_p_caller);
							}
						}
					}

					ImGui::EndMenu();
				}
			}
		}

		ImGui::EndMainMenuBar();
	}
}

void CMWIMGUIWidgetMenuBar::add_menu(char *t_cs_menu)
{
	bool t_b_existed = false;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			break;
		}
	}

	if (t_b_existed == false) {
		cmw_menu_s *t_p_menu = new cmw_menu_s();
		if (t_p_menu != NULL) {
			t_p_menu->m_pvec_menu_item = NULL;
			t_p_menu->m_n_selected = -1;
			memset(t_p_menu->m_cs_menu, 0, 128);
			memcpy(t_p_menu->m_cs_menu, t_cs_menu,
			       strlen(t_cs_menu));
			m_vec_menu.push_back(t_p_menu);
		}
	}
}

void CMWIMGUIWidgetMenuBar::set_menu_enable(char *t_cs_menu, bool t_b_enbale)
{
	bool t_b_existed = false;
	int t_n_index = -1;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			t_n_index = i;
			break;
		}
	}

	if (t_b_existed == false)
		return;

	vector<cmw_menu_item_s> *t_pvec_item = NULL;
	if (m_vec_menu.at(t_n_index)->m_pvec_menu_item == NULL)
		return;
	else
		t_pvec_item = m_vec_menu.at(t_n_index)->m_pvec_menu_item;

	if (t_pvec_item != NULL) {
		t_b_existed = false;
		int t_n_item_index = 0;
		for (int i = 0; i < t_pvec_item->size(); i++)
			t_pvec_item->at(i).m_b_enable = t_b_enbale;
	}
}

void CMWIMGUIWidgetMenuBar::add_menu_item(char *t_cs_menu, char *t_cs_menu_item)
{
	bool t_b_existed = false;
	int t_n_index = -1;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			t_n_index = i;
			break;
		}
	}

	if (t_b_existed == false)
		return;

	vector<cmw_menu_item_s> *t_pvec_item = NULL;
	if (m_vec_menu.at(t_n_index)->m_pvec_menu_item == NULL) {
		t_pvec_item = new vector<cmw_menu_item_s>();
		m_vec_menu.at(t_n_index)->m_pvec_menu_item = t_pvec_item;
	} else
		t_pvec_item = m_vec_menu.at(t_n_index)->m_pvec_menu_item;

	if (t_pvec_item != NULL) {
		t_b_existed = false;
		for (int i = 0; i < t_pvec_item->size(); i++) {
			if (strcmp(t_pvec_item->at(i).m_cs_menu_item,
				   t_cs_menu_item) == 0) {
				t_b_existed = true;
				break;
			}
		}
		if (t_b_existed == false) {
			cmw_menu_item_s t_menu_item;
			memset(t_menu_item.m_cs_menu_item, 0, 256);
			memcpy(t_menu_item.m_cs_menu_item, t_cs_menu_item,
			       strlen(t_cs_menu_item));
			t_menu_item.m_b_selected = false;
			t_menu_item.m_p_sub_menu = NULL;
			t_menu_item.m_b_enable = true;
			t_pvec_item->push_back(t_menu_item);
		}
	}
}

void CMWIMGUIWidgetMenuBar::add_menu_submenu(char *t_cs_menu,
					     char *t_cs_menu_sub_menu,
					     CMWIMGUIWidget *t_p_sub_menu)
{
	bool t_b_existed = false;
	int t_n_index = -1;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			t_n_index = i;
			break;
		}
	}

	if (t_b_existed == false)
		return;

	vector<cmw_menu_item_s> *t_pvec_item = NULL;
	if (m_vec_menu.at(t_n_index)->m_pvec_menu_item == NULL) {
		t_pvec_item = new vector<cmw_menu_item_s>();
		m_vec_menu.at(t_n_index)->m_pvec_menu_item = t_pvec_item;
	} else
		t_pvec_item = m_vec_menu.at(t_n_index)->m_pvec_menu_item;

	if (t_pvec_item != NULL) {
		t_b_existed = false;
		for (int i = 0; i < t_pvec_item->size(); i++) {
			if (strcmp(t_pvec_item->at(i).m_cs_menu_item,
				   t_cs_menu_sub_menu) == 0) {
				t_b_existed = true;
				break;
			}
		}
		if (t_b_existed == false) {
			cmw_menu_item_s t_menu_item;
			memset(t_menu_item.m_cs_menu_item, 0, 256);
			memcpy(t_menu_item.m_cs_menu_item, t_cs_menu_sub_menu,
			       strlen(t_cs_menu_sub_menu));
			t_menu_item.m_b_selected = false;
			t_menu_item.m_p_sub_menu = t_p_sub_menu;
			t_pvec_item->push_back(t_menu_item);
		}
	}
}

void CMWIMGUIWidgetMenuBar::set_menu_item_enable(char *t_cs_menu,
						 char *t_cs_menu_item,
						 bool t_b_enable)
{
	bool t_b_existed = false;
	int t_n_index = -1;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			t_n_index = i;
			break;
		}
	}

	if (t_b_existed == false)
		return;

	vector<cmw_menu_item_s> *t_pvec_item = NULL;
	if (m_vec_menu.at(t_n_index)->m_pvec_menu_item == NULL)
		return;
	else
		t_pvec_item = m_vec_menu.at(t_n_index)->m_pvec_menu_item;

	if (t_pvec_item != NULL) {
		t_b_existed = false;
		int t_n_item_index = 0;
		for (int i = 0; i < t_pvec_item->size(); i++) {
			if (strcmp(t_pvec_item->at(i).m_cs_menu_item,
				   t_cs_menu_item) == 0) {
				t_b_existed = true;
				t_n_item_index = i;
				break;
			}
		}
		if (t_b_existed == false)
			return;
		else
			t_pvec_item->at(t_n_item_index).m_b_enable = t_b_enable;
	}
}

void CMWIMGUIWidgetMenuBar::set_menu_sub_menu_enable(char *t_cs_menu,
						     char *t_cs_menu_sub_menu,
						     bool t_b_enable)
{
	set_menu_item_enable(t_cs_menu, t_cs_menu_sub_menu, t_b_enable);
}

void CMWIMGUIWidgetMenuBar::set_menu_item_checked(char *t_cs_menu,
						  char *t_cs_menu_item,
						  bool t_b_checked)
{
	bool t_b_existed = false;
	int t_n_index = -1;
	for (int i = 0; i < m_vec_menu.size(); i++) {
		if (strcmp(m_vec_menu.at(i)->m_cs_menu, t_cs_menu) == 0) {
			t_b_existed = true;
			t_n_index = i;
			break;
		}
	}

	if (t_b_existed == false)
		return;

	vector<cmw_menu_item_s> *t_pvec_item = NULL;
	if (m_vec_menu.at(t_n_index)->m_pvec_menu_item == NULL)
		return;
	else
		t_pvec_item = m_vec_menu.at(t_n_index)->m_pvec_menu_item;

	if (t_pvec_item != NULL) {
		t_b_existed = false;
		int t_n_item_index = 0;
		for (int i = 0; i < t_pvec_item->size(); i++) {
			if (strcmp(t_pvec_item->at(i).m_cs_menu_item,
				   t_cs_menu_item) == 0) {
				t_b_existed = true;
				t_n_item_index = i;
				break;
			}
		}
		if (t_b_existed == false)
			return;
		else {
			t_pvec_item->at(t_n_item_index).m_b_selected =
				t_b_checked;
			if (t_b_checked)
				m_vec_menu.at(t_n_index)->m_n_selected =
					t_n_item_index;
			else
				m_vec_menu.at(t_n_index)->m_n_selected = -1;
		}
	}
}

void CMWIMGUIWidgetMenuBar::set_menu_sub_menu_checked(char *t_cs_menu,
						      char *t_cs_menu_sub_menu,
						      bool t_b_checked)
{
	set_menu_item_checked(t_cs_menu, t_cs_menu_sub_menu, t_b_checked);
}

void CMWIMGUIWidgetMenuBar::set_menu_selected_callback(
	callback_menu_item_selected t_callback, void *t_p_param)
{
	m_callback_selected = t_callback;
	m_p_caller = t_p_param;
}
