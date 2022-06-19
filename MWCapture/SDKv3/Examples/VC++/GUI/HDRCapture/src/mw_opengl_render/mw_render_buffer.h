/************************************************************************************************/
// mw_render_buffer.h : interface of the CMWRenderBuffer class

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

#ifndef MW_RENDER_BUFFER_H
#define MW_RENDER_BUFFER_H


#include <queue>
using namespace std;

#include "Windows.h"

struct cmw_buffer_s
{
	unsigned char*			m_p_buffer;
	int						m_n_buffer_size;
	int						m_n_data_size;
};

class CMWRenderBuffer
{
public:
	CMWRenderBuffer();
	~CMWRenderBuffer();

public:
	bool allocate_buffer(int t_n_size);

	// get buffer free to fill
	cmw_buffer_s* get_buffer_to_fill();
	void put_buffer_filled(cmw_buffer_s* t_p_buffer);

	cmw_buffer_s* get_buffer_render();
	void put_buffer_rended(cmw_buffer_s* t_p_buffer);

	int get_buffer_num();
	cmw_buffer_s* get_buffer_by_index(int t_n_index);

	void clear_data();
	bool is_clear();

protected:
	int						m_n_index_current_buffer;
	int						m_n_size_current_buffer;

	cmw_buffer_s			m_array_buffer[4];
	int						m_n_num_buffer;
	queue<cmw_buffer_s*>	m_queue_buffer_free;
	queue<cmw_buffer_s*>	m_queue_buffer_full;

	bool					m_b_clear;

	CRITICAL_SECTION		m_cs_lock;
};


#endif