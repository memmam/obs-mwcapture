/************************************************************************************************/
// mw_render_buffer.cpp : implementation of the CMWRenderBuffer class

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

#include "mw_render_buffer.h"
#include "stdio.h"
#include "string.h"

CMWRenderBuffer::CMWRenderBuffer()
{
	m_n_index_current_buffer = 0;
	m_n_size_current_buffer = 0;

	m_n_num_buffer = 4;
	for (int i = 0; i < m_n_num_buffer; i++) {
		m_array_buffer[i].m_p_buffer = NULL;
		m_array_buffer[i].m_n_buffer_size = 0;
		m_array_buffer[i].m_n_data_size = 0;
	}

	m_b_clear = false;

	InitializeCriticalSection(&m_cs_lock);
}

CMWRenderBuffer::~CMWRenderBuffer()
{
	for (int i = 0; i < m_n_num_buffer; i++) {
		if (m_array_buffer[i].m_p_buffer != NULL) {
			delete m_array_buffer[i].m_p_buffer;
			m_array_buffer[i].m_p_buffer = NULL;
		}
		m_array_buffer[i].m_n_buffer_size = 0;
		m_array_buffer[i].m_n_data_size = 0;
	}

	DeleteCriticalSection(&m_cs_lock);
}

bool CMWRenderBuffer::allocate_buffer(int t_n_size)
{
	bool t_b_ret = false;
	if (t_n_size <= 0)
		return t_b_ret;

	EnterCriticalSection(&m_cs_lock);

	// clear queue
	while (!m_queue_buffer_free.empty())
		m_queue_buffer_free.pop();

	while (!m_queue_buffer_full.empty())
		m_queue_buffer_full.pop();

	for (int i = 0; i < m_n_num_buffer; i++) {
		if (m_array_buffer[i].m_p_buffer != NULL) {
			delete m_array_buffer[i].m_p_buffer;
			m_array_buffer[i].m_p_buffer = NULL;
		}
		m_array_buffer[i].m_n_buffer_size = 0;
		m_array_buffer[i].m_n_data_size = 0;
	}

	for (int i = 0; i < m_n_num_buffer; i++) {
		m_array_buffer[i].m_p_buffer = new unsigned char[t_n_size];
		if (m_array_buffer[i].m_p_buffer != NULL) {
			m_array_buffer[i].m_n_buffer_size = t_n_size;
			m_array_buffer[i].m_n_data_size = 0;

			m_queue_buffer_free.push(&m_array_buffer[i]);
			t_b_ret = true;
		}
	}
	cmw_buffer_s *t_p_buffer = m_queue_buffer_free.front();
	m_queue_buffer_free.pop();
	m_queue_buffer_full.push(t_p_buffer);

	LeaveCriticalSection(&m_cs_lock);

	return t_b_ret;
}

cmw_buffer_s *CMWRenderBuffer::get_buffer_to_fill()
{
	cmw_buffer_s *t_p_buffer = NULL;

	EnterCriticalSection(&m_cs_lock);

	if (!m_queue_buffer_free.empty()) {
		t_p_buffer = m_queue_buffer_free.front();
		m_queue_buffer_free.pop();
	} else {
		if (!m_queue_buffer_full.empty()) {
			t_p_buffer = m_queue_buffer_full.front();
			m_queue_buffer_full.pop();
		}
	}

	LeaveCriticalSection(&m_cs_lock);

	return t_p_buffer;
}

void CMWRenderBuffer::put_buffer_filled(cmw_buffer_s *t_p_buffer)
{
	if (t_p_buffer == NULL)
		return;

	EnterCriticalSection(&m_cs_lock);

	m_queue_buffer_full.push(t_p_buffer);

	LeaveCriticalSection(&m_cs_lock);
}

cmw_buffer_s *CMWRenderBuffer::get_buffer_render()
{
	cmw_buffer_s *t_p_buffer = NULL;

	EnterCriticalSection(&m_cs_lock);

	if (!m_queue_buffer_full.empty()) {
		t_p_buffer = m_queue_buffer_full.front();
		m_queue_buffer_full.pop();
	} else {
		if (!m_queue_buffer_free.empty()) {
			t_p_buffer = m_queue_buffer_free.front();
			m_queue_buffer_free.pop();
		}
	}

	LeaveCriticalSection(&m_cs_lock);

	return t_p_buffer;
}

void CMWRenderBuffer::put_buffer_rended(cmw_buffer_s *t_p_buffer)
{
	if (t_p_buffer == NULL)
		return;

	EnterCriticalSection(&m_cs_lock);

	if (m_queue_buffer_full.empty())
		m_queue_buffer_full.push(t_p_buffer);
	else
		m_queue_buffer_free.push(t_p_buffer);

	LeaveCriticalSection(&m_cs_lock);
}

int CMWRenderBuffer::get_buffer_num()
{
	return m_n_num_buffer;
}

cmw_buffer_s *CMWRenderBuffer::get_buffer_by_index(int t_n_index)
{
	cmw_buffer_s *t_p_buffer = NULL;

	if (t_n_index >= 0 && t_n_index < m_n_num_buffer)
		t_p_buffer = &m_array_buffer[t_n_index];

	return t_p_buffer;
}

void CMWRenderBuffer::clear_data()
{
	for (int i = 0; i < 2; i++) {
		if (m_array_buffer[i].m_p_buffer != NULL)
			memset(m_array_buffer[i].m_p_buffer, 0,
			       m_array_buffer[i].m_n_buffer_size);
	}

	m_b_clear = true;
}

bool CMWRenderBuffer::is_clear()
{
	return m_b_clear;
}
