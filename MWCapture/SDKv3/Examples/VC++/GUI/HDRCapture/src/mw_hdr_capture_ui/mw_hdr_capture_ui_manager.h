#ifndef MW_HDR_CAPTURE_UI_MANAGER_H
#define MW_HDR_CAPTURE_UI_MANAGER_H

#include "../mw_imgui_widget/mw_hdr_capture_menubar.h"
#include "../mw_imgui_widget/mw_hdr_value.h"
#include "../mw_imgui_widget/mw_encoder_adapter.h"
#include "../mw_imgui_widget/mw_hdr_overlay_text.h"

class CMWHDRCaptureUIManager {
public:
	CMWHDRCaptureUIManager();
	~CMWHDRCaptureUIManager();

public:
	void add_widget(CMWIMGUIWidget *t_p_wid);
	void update_ui();

private:
	vector<CMWIMGUIWidget *> m_vec_widget;
};

#endif