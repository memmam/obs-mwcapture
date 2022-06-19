/************************************************************************************************/
// MWMultiDSBuffer.cpp : implementation of the MWMultiDSBuffer class

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

#include "stdafx.h"
#include "MWMultiDSBuffer.h"
#include <stdlib.h>

MWMultiDSBuffer::MWMultiDSBuffer()
{
	m_u32_size=0;
	m_u16_frame_count=0;
	m_u16_frame_read_pos=0;
	m_u16_frame_writ_pos=0;

	m_p_audio_block=NULL;
	m_audioout_block.m_block_state=MW_AUDIO_BLOCK_EMPTY;
	m_audioout_block.m_p_data=NULL;
	m_audioout_block.m_u32_size=0;

	InitializeCriticalSection(&m_cs_frame);
}

MWMultiDSBuffer::~MWMultiDSBuffer()
{
	DeleteCriticalSection(&m_cs_frame);
}

mw_multi_ds_status_e MWMultiDSBuffer::create_audio_buffers(
	uint32_t t_u32_size,
	uint16_t t_u16_frame_count,
	uint32_t t_u32_limit
	)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	uint32_t t_u32_max_size=MW_MAX_SAMPLERATE*MW_MAX_CHANNELS*MW_MAX_BITS_PER_SAMPLE/8;
	if(t_u32_max_size<t_u32_size){
		t_ret=MW_AUDIO_BUF_SIZE_OVERFLOW;
		return t_ret;
	}

	m_u32_size=t_u32_size;
	m_u32_limit=t_u32_limit;

	if(t_u16_frame_count>MW_MAX_BUF_NUM){
		t_ret=MW_AUDIO_BUF_NUM_OVERFLOW;
		return t_ret;
	}
	m_u16_frame_count=t_u16_frame_count;


	m_p_audio_block=new mw_audio_block_t[m_u16_frame_count];
	for(int i=0;i<m_u16_frame_count;i++){
		m_p_audio_block[i].m_p_data=new uint8_t[m_u32_size];
		memset(m_p_audio_block[i].m_p_data,0,m_u32_size);
		m_p_audio_block[i].m_u32_size=0;
		m_p_audio_block[i].m_block_state=MW_AUDIO_BLOCK_EMPTY;
	}

	m_u16_frame_read_pos=0;
	m_u16_frame_writ_pos=0;

	m_audioout_block.m_block_state=MW_AUDIO_BLOCK_EMPTY;
	m_audioout_block.m_p_data=new uint8_t[m_u32_size];
	m_audioout_block.m_u32_size=0;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSBuffer::destroy_audio_buffers()
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;

	if(m_u32_size==0||m_u16_frame_count==0||m_p_audio_block==NULL)
		return t_ret;

	for(int i=0;i<m_u16_frame_count;i++){
		if(m_p_audio_block[i].m_p_data!=NULL)
			delete m_p_audio_block[i].m_p_data;

		m_p_audio_block[i].m_p_data=NULL;
		m_p_audio_block[i].m_u32_size=0;
	}
	delete m_p_audio_block;
	m_p_audio_block=NULL;

	m_audioout_block.m_block_state=MW_AUDIO_BLOCK_EMPTY;
	if(m_audioout_block.m_p_data!=NULL){
		delete m_audioout_block.m_p_data;
		m_audioout_block.m_p_data=NULL;
	}
	m_audioout_block.m_u32_size=0;

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSBuffer::put_audio_frames(uint8_t *t_p_frame,uint32_t t_u32_size)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	if(t_p_frame==NULL){
		t_ret=MW_AUDIO_BUF_NULL;
		return t_ret;
	}

	if(!(t_u32_size>0&&t_u32_size<=m_u32_size)){
		t_ret=MW_AUDIO_BUF_SIZE_ERROR;
		return t_ret;
	}

	EnterCriticalSection(&m_cs_frame);

	mw_audio_block_state_e t_state=m_p_audio_block[m_u16_frame_writ_pos].m_block_state;
	uint8_t *t_p_des=m_p_audio_block[m_u16_frame_writ_pos].m_p_data+m_p_audio_block[m_u16_frame_writ_pos].m_u32_size;
	if(t_state==MW_AUDIO_BLOCK_BUFFERD){
		t_ret=MW_AUDIO_BUF_OVERRUN;
	}
	else{
		mw_audio_block_t *t_p_block=&m_p_audio_block[m_u16_frame_writ_pos];
		int t_n_more=t_p_block->m_u32_size+t_u32_size-m_u32_limit;
		if(t_n_more<0){
			memcpy(t_p_des,t_p_frame,t_u32_size);
			t_p_block->m_u32_size+=t_u32_size;
			t_p_block->m_block_state=MW_AUDIO_BLOCK_BUFFERING;
		}
		else{
			int t_n_cp=m_u32_limit-t_p_block->m_u32_size;
			memcpy(t_p_des,t_p_frame,t_n_cp);
			t_p_block->m_u32_size+=t_n_cp;
			t_p_block->m_block_state=MW_AUDIO_BLOCK_BUFFERD;

			m_u16_frame_writ_pos++;
			m_u16_frame_writ_pos=m_u16_frame_writ_pos%m_u16_frame_count;
			if(t_n_more!=0){
				t_p_block=&m_p_audio_block[m_u16_frame_writ_pos];
				t_p_block->m_u32_size=0;
				t_p_des=m_p_audio_block[m_u16_frame_writ_pos].m_p_data+m_p_audio_block[m_u16_frame_writ_pos].m_u32_size;
				uint8_t *t_p_src=t_p_frame+t_n_cp;
				memcpy(t_p_des,t_p_src,t_n_more);
				t_p_block->m_u32_size+=t_n_more;
				t_p_block->m_block_state==MW_AUDIO_BLOCK_BUFFERING;
			}
		}
	}

	LeaveCriticalSection(&m_cs_frame);

	return t_ret;
}

mw_multi_ds_status_e MWMultiDSBuffer::get_audio_frames(mw_audio_block_t* &t_p_block)
{
	mw_multi_ds_status_e t_ret=MW_AUDIO_NO_ERROR;
	EnterCriticalSection(&m_cs_frame);

	mw_audio_block_state_e t_state=m_p_audio_block[m_u16_frame_read_pos].m_block_state;
	if(t_state!=MW_AUDIO_BLOCK_BUFFERD){
		t_ret=MW_AUDIO_BUF_EMPTY;
		t_p_block=NULL;
	}
	else{
		uint8_t *t_p_src=m_p_audio_block[m_u16_frame_read_pos].m_p_data;
		uint8_t *t_p_des=m_audioout_block.m_p_data;
		uint32_t t_u32_size=m_p_audio_block[m_u16_frame_read_pos].m_u32_size;
		m_audioout_block.m_u32_size=t_u32_size;
		memcpy(t_p_des,t_p_src,t_u32_size);

		t_p_block=&m_audioout_block;

		m_p_audio_block[m_u16_frame_read_pos].m_block_state=MW_AUDIO_BLOCK_EMPTY;
		m_p_audio_block[m_u16_frame_read_pos].m_u32_size=0;
		m_u16_frame_read_pos++;
		m_u16_frame_read_pos=m_u16_frame_read_pos%m_u16_frame_count;
	}

	LeaveCriticalSection(&m_cs_frame);

	return t_ret;
}
