/************************************************************************************************/
// mw_imgui_widget.h : interface of the CMWIMGUIWidget class

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

#ifndef MW_IMGUI_WIDGET_H
#define MW_IMGUI_WIDGET_H

#include "imgui.h"
#include "imgui_glfw/imgui_impl_glfw.h"
#include "imgui_opengl3/imgui_impl_opengl3.h"

// using gl3w to load opengl functions
#include "GL/gl3w.h"

// include glfw3.h after opengl definitions
#include "GLFW/glfw3.h"

class CMWIMGUIUIManager;

class CMWIMGUIWidget
{
public:
	CMWIMGUIWidget(CMWIMGUIUIManager* t_p_manager);
	~CMWIMGUIWidget();

public:
	virtual void update_widget() = 0;
	virtual void reset_ui();

protected:
	CMWIMGUIUIManager*		m_ui_manager;

};

#endif