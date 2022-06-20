/************************************************************************************************/
// MWMultiDSBuffer.h : interface of the MWMultiDSBuffer class

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

#ifndef MWMULTIDSBUFFER_H
#define MWMULTIDSBUFFER_H

#include "MWMultiDSDefine.h"
#include <stdint.h>

#define MW_MAX_BUF_NUM	60
#define MW_MIN_BUF_NUM	3

#define MW_MAX_SAMPLERATE		192000
#define MW_MAX_CHANNELS			8
#define MW_MAX_BITS_PER_SAMPLE	32

class MWMultiDSBuffer
{
public:
	MWMultiDSBuffer();
	~MWMultiDSBuffer();

public:
	mw_multi_ds_status_e create_audio_buffers(
		uint32_t t_u32_size,
		uint16_t t_u16_frame_count,
		uint32_t t_u32_limit
		);

	mw_multi_ds_status_e destroy_audio_buffers();

	mw_multi_ds_status_e put_audio_frames(uint8_t *t_p_frame,uint32_t t_u32_size);

	mw_multi_ds_status_e get_audio_frames(mw_audio_block_t* &t_p_block);

protected:
	uint32_t			m_u32_size;
	uint32_t			m_u32_limit;
	uint16_t			m_u16_frame_count;
	uint16_t			m_u16_frame_read_pos;
	uint16_t			m_u16_frame_writ_pos;

	mw_audio_block_t	*m_p_audio_block;
	mw_audio_block_t	m_audioout_block;

	CRITICAL_SECTION	m_cs_frame;
};

#endif