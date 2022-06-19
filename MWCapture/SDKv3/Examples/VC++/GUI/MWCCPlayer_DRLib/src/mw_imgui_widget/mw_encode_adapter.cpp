/************************************************************************************************/
// mw_encode_adapter.cpp : implementation of the CMWEncoderAdapter class

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

#include "mw_encode_adapter.h"

CMWEncoderAdapter::CMWEncoderAdapter(CMWCCPlayerUIManager* t_p_manager):
	CMWIMGUIWidget(t_p_manager)
{
	m_vec_gpu.clear();
	m_n_index = 0;
	m_pp_cs_adapters = NULL;
}

CMWEncoderAdapter::~CMWEncoderAdapter()
{
	if(NULL!=m_pp_cs_adapters){
		delete []m_pp_cs_adapters;
		m_pp_cs_adapters = NULL;
	}
}

void CMWEncoderAdapter::update_widget()
{
	if(!m_vec_gpu.empty()&&m_pp_cs_adapters)
		ImGui::Combo("Adapters",&m_n_index,m_pp_cs_adapters,m_vec_gpu.size());
	else
		ImGui::Text("No adapter on this device encoding.");
}

int CMWEncoderAdapter::get_selected_adapter()
{
	return m_n_index;
}

void 
CMWEncoderAdapter::set_adapters(
	vector<mw_venc_gpu_info_t> t_vec_gpu,
	vector<int> t_vec_index)
{
	m_vec_gpu = t_vec_gpu;
	m_vec_index = t_vec_index;
	if(m_pp_cs_adapters!=NULL){
		delete []m_pp_cs_adapters;
		m_pp_cs_adapters = NULL;
	}

	int t_n_size = m_vec_gpu.size();
	if(t_n_size>0){
		m_pp_cs_adapters = new char*[t_n_size];
		if(m_pp_cs_adapters){
			for(int i=0;i<t_n_size;i++){
				m_pp_cs_adapters[i]= new char[128];
				memset(m_pp_cs_adapters[i],0,128);
				sprintf(m_pp_cs_adapters[i],"%u-%s",m_vec_index.at(i),m_vec_gpu.at(i).gpu_name);
			}
		}
	}

}

