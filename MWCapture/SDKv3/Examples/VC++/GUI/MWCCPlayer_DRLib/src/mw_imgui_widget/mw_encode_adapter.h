/************************************************************************************************/
// mw_encode_adapter.h : interface of the CMWEncoderAdapter class

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

#ifndef MW_ENCODE_ADAPTER_H
#define MW_ENCODE_ADAPTER_H

#include "mw_imgui_widget.h"
#include "LibMWVenc/mw_venc.h"

class CMWEncoderAdapter : public CMWIMGUIWidget {
public:
	CMWEncoderAdapter(CMWCCPlayerUIManager *t_p_manager);
	~CMWEncoderAdapter();

public:
	void update_widget();

	int get_selected_adapter();
	void set_adapters(vector<mw_venc_gpu_info_t> t_vec_gpu,
			  vector<int> t_vec_index);

protected:
	vector<mw_venc_gpu_info_t> m_vec_gpu;
	vector<int> m_vec_index;
	int m_n_index;

	char **m_pp_cs_adapters;
};

#endif
