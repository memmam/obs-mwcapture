/************************************************************************************************/
// mw_cc_player_menubar.h : interface of the CMCCPlayerMenuBar class

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

#ifndef MW_CC_PLAYER_MENUBAR_H
#define MW_CC_PLAYER_MENUBAR_H

#include "mw_imgui_widget.h"

struct cmw_menu_item_s
{
	char						m_cs_menu_item[256];
	CMWIMGUIWidget*				m_p_sub_menu;
	bool						m_b_selected;
	bool						m_b_enable;
};

struct cmw_menu_s {
	char						m_cs_menu[128];
	vector<cmw_menu_item_s>*	m_pvec_menu_item;
	int							m_n_selected;
};

typedef void(*callback_menu_item_selected)(char* t_cs_menu,char* t_cs_menu_item,int t_n_index,void* t_p_param);


class CMCCPlayerMenuBar:public CMWIMGUIWidget
{
public:
	CMCCPlayerMenuBar(CMWCCPlayerUIManager* t_p_manager);
	~CMCCPlayerMenuBar();

public:
	void update_widget();

	void add_menu(char* t_cs_menu);
	void set_menu_enable(char* t_cs_menu,bool t_b_enbale);
	void add_menu_item(char* t_cs_menu,char* t_cs_menu_item);
	void add_menu_submenu(char* t_cs_menu,char* t_cs_menu_sub_menu, CMWIMGUIWidget* t_p_sub_menu);
	void set_menu_item_enable(char* t_cs_menu, char* t_cs_menu_item,bool t_b_enable);
	void set_menu_sub_menu_enable(char* t_cs_menu, char* t_cs_menu_sub_menu, bool t_b_enable);
	void set_menu_item_checked(char* t_cs_menu, char* t_cs_menu_item, bool t_b_checked);
	void set_menu_sub_menu_checked(char* t_cs_menu, char* t_cs_menu_sub_menu, bool t_b_checked);

	void set_menu_selected_callback(callback_menu_item_selected t_callback, void* t_p_param);
protected:
	vector<cmw_menu_s*>					m_vec_menu;
	callback_menu_item_selected			m_callback_selected;
	void*								m_p_caller;
};

#endif