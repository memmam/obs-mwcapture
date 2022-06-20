/************************************************************************************************/
// mw_hdr_encoder.cpp : implementation of the CMWHDREncoder class

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

#include "mw_hdr_encoder.h"

CMWHDREncoder::CMWHDREncoder()
{
	m_h_encoder = nullptr;
	m_b_is_encoding = false;
}

CMWHDREncoder::~CMWHDREncoder()
{
	m_b_is_encoding = false;
	if (m_h_encoder != nullptr) {
		close_encoder();
		m_h_encoder = nullptr;
	}
}

bool CMWHDREncoder::open_encoder(mw_venc_platform_t platform,
				 mw_venc_param_t *p_param,
				 MW_ENCODER_CALLBACK frame_callback,
				 void *user_ptr, int t_n_index)
{
	bool t_b_ret = false;

	if (m_h_encoder != nullptr)
		return t_b_ret;

	m_b_is_encoding = false;

	if (t_n_index < 0)
		m_h_encoder = mw_venc_create(platform, p_param, frame_callback,
					     user_ptr);
	else
		m_h_encoder = mw_venc_create_by_index(t_n_index, p_param,
						      frame_callback, user_ptr);

	if (m_h_encoder == nullptr)
		return t_b_ret;

	m_b_is_encoding = true;
	t_b_ret = true;
	return t_b_ret;
}

bool CMWHDREncoder::close_encoder()
{
	bool t_b_ret = false;

	if (m_h_encoder == nullptr) {
		t_b_ret = true;
		return t_b_ret;
	}

	mw_venc_status_t t_stat = mw_venc_destory(m_h_encoder);
	if (t_stat == MW_VENC_STATUS_SUCCESS) {
		m_h_encoder = nullptr;
		m_b_is_encoding = false;
		t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWHDREncoder::put_frame_ex(uint8_t *p_frame, uint64_t t_u64_timestamp)
{
	bool t_b_ret = false;
	if (p_frame == nullptr)
		return t_b_ret;

	if (!m_b_is_encoding)
		return t_b_ret;

	if (m_h_encoder != nullptr) {
		mw_venc_status t_stat = mw_venc_put_frame_ex(
			m_h_encoder, p_frame, t_u64_timestamp / 10000);
		if (t_stat == MW_VENC_STATUS_SUCCESS)
			t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWHDREncoder::get_property(mw_venc_property_t param, void *args)
{
	bool t_b_ret = false;
	if (args == nullptr)
		return t_b_ret;

	if (!m_b_is_encoding)
		return t_b_ret;

	if (m_h_encoder != nullptr) {
		mw_venc_status t_stat =
			mw_venc_get_property(m_h_encoder, param, args);
		if (t_stat == MW_VENC_STATUS_SUCCESS)
			t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWHDREncoder::set_property(mw_venc_property_t param, void *args)
{
	bool t_b_ret = false;
	if (args == nullptr)
		return t_b_ret;

	if (!m_b_is_encoding)
		return t_b_ret;

	if (m_h_encoder != nullptr) {
		mw_venc_status t_stat =
			mw_venc_set_property(m_h_encoder, param, args);
		if (t_stat == MW_VENC_STATUS_SUCCESS)
			t_b_ret = true;
	}

	return t_b_ret;
}

bool CMWHDREncoder::get_is_encoding()
{
	return m_b_is_encoding;
}

void CMWHDREncoder::set_is_encoding(bool t_b_encoding)
{
	m_b_is_encoding = t_b_encoding;
}
