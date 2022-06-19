/************************************************************************************************/
// mw_encoder.h : interface of the CMWEncoder class

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

#ifndef MW_ENCODER_H
#define MW_ENCODER_H

#include "LibMWVenc/mw_venc.h"

class CMWEncoder
{
public:
	CMWEncoder();
	~CMWEncoder();

public:
	bool open_encoder(
		mw_venc_platform_t platform,
		mw_venc_param_t *p_param,
		MW_ENCODER_CALLBACK frame_callback,
		void *user_ptr,
		int t_n_index = -1);

	bool close_encoder();

	bool put_frame(uint8_t *p_frame);

	bool get_property(
		mw_venc_property_t param,
		void *args);

	bool set_property(
		mw_venc_property_t param,
		void *args);

	bool get_is_encoding();

	void set_is_encoding(bool t_b_encoding);

protected:
	mw_venc_handle_t		m_h_encoder;
	bool					m_b_is_encoding;

};

#endif