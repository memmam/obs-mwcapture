#include "mw_hdr_capture_ui_manager.h"

CMWHDRCaptureUIManager::CMWHDRCaptureUIManager() {}

CMWHDRCaptureUIManager::~CMWHDRCaptureUIManager() {}

void CMWHDRCaptureUIManager::add_widget(CMWIMGUIWidget *t_p_wid)
{
	if (t_p_wid != NULL)
		m_vec_widget.push_back(t_p_wid);
}

void CMWHDRCaptureUIManager::update_ui()
{
	for (int i = 0; i < m_vec_widget.size(); i++)
		m_vec_widget.at(i)->update_widget();
}
