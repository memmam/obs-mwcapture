/************************************************************************************************/
// mw_encoder.cpp : implementation of the CMWEncoder class

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

#include "mw_encoder.h"

CMWEncoder::CMWEncoder()
{
	m_h_encoder = nullptr;
	m_b_is_encoding = false;
}

CMWEncoder::~CMWEncoder()
{
	m_b_is_encoding = false;
	if(m_h_encoder){
		close_encoder();
		m_h_encoder = nullptr;
	}
}

bool 
CMWEncoder::open_encoder(
	mw_venc_platform_t t_platform, 
	mw_venc_param_t* t_p_param, 
	MW_ENCODER_CALLBACK t_frame_callback, 
	void* t_p_user, 
	int t_n_index /*= -1 */)
{
	bool t_b_ret = false;

	if(m_h_encoder)
		return t_b_ret;

	m_b_is_encoding = false;

	if(t_n_index<0)
		m_h_encoder = mw_venc_create(t_platform,t_p_param,t_frame_callback,t_p_user);
	else
		m_h_encoder = mw_venc_create_by_index(t_n_index,t_p_param,t_frame_callback,t_p_user);

	if(nullptr == m_h_encoder)
		return t_b_ret;

	m_b_is_encoding = true;
	t_b_ret = true;

	return t_b_ret;
}

bool CMWEncoder::close_encoder()
{
	bool t_b_ret = false;

	if(nullptr == m_h_encoder){
		t_b_ret = true;
		return t_b_ret;
	}

	mw_venc_status_t t_stat = mw_venc_destory(m_h_encoder);
	if(MW_VENC_STATUS_SUCCESS == t_stat){
		m_h_encoder = nullptr;
		m_b_is_encoding = false;
		t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWEncoder::put_frame_ex(uint8_t* t_p_frame, uint64_t t_u64_timestamp)
{
	bool t_b_ret = false;

	if(nullptr == t_p_frame)
		return t_b_ret;

	if(!m_b_is_encoding)
		return t_b_ret;

	if(nullptr != m_h_encoder){
		mw_venc_status_t t_stat = mw_venc_put_frame_ex(m_h_encoder,t_p_frame,t_u64_timestamp/10000);
		if(MW_VENC_STATUS_SUCCESS == t_stat)
			t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWEncoder::get_is_encoding()
{
	return m_b_is_encoding;
}

void CMWEncoder::set_is_encoding(bool t_b_encoding)
{
	m_b_is_encoding = t_b_encoding;
}
