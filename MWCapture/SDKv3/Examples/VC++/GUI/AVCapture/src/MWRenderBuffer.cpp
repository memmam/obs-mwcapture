#include "stdafx.h"
#include "MWRenderBuffer.h"

CMWRenderBuffer::CMWRenderBuffer()
{
	m_u32_id_cur_buf = 0;
	m_u32_size_cur_buf = 0;

	m_u32_num_buf = 30;
	m_u32_id_write = 0;
	m_u32_id_read_render = 0;
	m_u32_id_read_encode = 0;
	m_u32_dur_render = 0;
	m_u32_dur_encode = 0;
	m_pa_buf = NULL;
	m_pa_buf = new mw_buffer_t[m_u32_num_buf];
	if (m_pa_buf != NULL) {
		for (int i = 0; i < m_u32_num_buf; i++) {
			m_pa_buf[i].m_p_buf = NULL;
			m_pa_buf[i].m_u32_size_buf = 0;
			m_pa_buf[i].m_u32_size_data = 0;
		}
	}

	m_b_clear = false;

	InitializeCriticalSection(&m_cs_lock);
}

CMWRenderBuffer::~CMWRenderBuffer()
{
	free_buf();
	DeleteCriticalSection(&m_cs_lock);
}

bool CMWRenderBuffer::alloc_buf(uint32_t t_u32_size)
{
	bool t_b_ret = false;

	if (t_u32_size == 0 || m_pa_buf == NULL)
		return t_b_ret;

	for (int i = 0; i < m_u32_num_buf; i++) {
		if (m_pa_buf[i].m_p_buf != NULL) {
			delete[] m_pa_buf[i].m_p_buf;
			m_pa_buf[i].m_p_buf = NULL;
		}
		m_pa_buf[i].m_u32_size_buf = 0;
		m_pa_buf[i].m_u32_size_data = 0;
	}

	t_b_ret = true;
	for (int i = 0; i < m_u32_num_buf; i++) {
		m_pa_buf[i].m_p_buf = new uint8_t[t_u32_size];
		if (m_pa_buf[i].m_p_buf != NULL) {
			m_pa_buf[i].m_u32_size_buf = t_u32_size;
			m_pa_buf[i].m_u32_size_data = 0;
		} else {
			t_b_ret = false;
			break;
		}
	}

	if (!t_b_ret) {
		free_buf();
	}

	return t_b_ret;
}

void CMWRenderBuffer::free_buf()
{
	if (m_pa_buf != NULL) {
		for (int i = 0; i < m_u32_num_buf; i++) {
			if (m_pa_buf[i].m_p_buf != NULL) {
				delete[] m_pa_buf[i].m_p_buf;
				m_pa_buf[i].m_p_buf = NULL;
			}
			m_pa_buf[i].m_u32_size_buf = 0;
			m_pa_buf[i].m_u32_size_data = 0;
		}
		delete[] m_pa_buf;
		m_pa_buf = NULL;
	}
}

mw_buffer_t *CMWRenderBuffer::get_buf_to_fill()
{
	mw_buffer_t *t_p_buf = NULL;

	EnterCriticalSection(&m_cs_lock);
	t_p_buf = &m_pa_buf[m_u32_id_write];
	LeaveCriticalSection(&m_cs_lock);

	return t_p_buf;
}

void CMWRenderBuffer::put_buf_filled()
{
	EnterCriticalSection(&m_cs_lock);
	m_u32_id_write++;
	m_u32_dur_render = 1;
	m_u32_dur_encode = 1;
	m_u32_id_write = m_u32_id_write % m_u32_num_buf;
	LeaveCriticalSection(&m_cs_lock);
}

mw_buffer_t *CMWRenderBuffer::get_buf_render(uint32_t &t_id_render)
{
	mw_buffer_t *t_p_buf = NULL;

	//EnterCriticalSection(&m_cs_lock);

	if (m_u32_dur_render == 0) {
		t_p_buf = NULL;
		t_id_render = 0;
	} else {
		m_u32_id_read_render =
			(m_u32_id_write + m_u32_num_buf - m_u32_dur_render) %
			m_u32_num_buf;
		t_p_buf = &m_pa_buf[m_u32_id_read_render];
		t_id_render = m_u32_id_read_render;
	}

	//LeaveCriticalSection(&m_cs_lock);

	return t_p_buf;
}

mw_buffer_t *CMWRenderBuffer::get_buf_encode(uint32_t &t_id_encode)
{
	mw_buffer_t *t_p_buf = NULL;

	//EnterCriticalSection(&m_cs_lock);

	if (m_u32_dur_encode == 0) {
		t_p_buf = NULL;
		t_id_encode = 0;
	} else {
		m_u32_id_read_encode =
			(m_u32_id_write + m_u32_num_buf - m_u32_dur_encode) %
			m_u32_num_buf;
		t_p_buf = &m_pa_buf[m_u32_id_read_encode];
		t_id_encode = m_u32_id_read_encode;
	}

	//LeaveCriticalSection(&m_cs_lock);

	return t_p_buf;
}

uint32_t CMWRenderBuffer::get_buf_num()
{
	return m_u32_num_buf;
}

mw_buffer_t *CMWRenderBuffer::get_buf_by_id(uint32_t t_u32_id)
{
	mw_buffer_t *t_p_buf = NULL;

	if (t_u32_id >= 0 && t_u32_id < m_u32_num_buf)
		t_p_buf = &m_pa_buf[t_u32_id];

	return t_p_buf;
}

void CMWRenderBuffer::clear_data()
{
	for (int i = 0; i < m_u32_num_buf; i++) {
		if (m_pa_buf[i].m_p_buf != NULL)
			memset(m_pa_buf[i].m_p_buf, 0,
			       m_pa_buf[i].m_u32_size_buf);
	}

	m_u32_id_write = 0;
	m_u32_id_read_render = 0;
	m_u32_id_read_encode = 0;
	m_u32_dur_render = 0;
	m_u32_dur_encode = 0;

	m_b_clear = true;
}

bool CMWRenderBuffer::is_clear()
{
	return m_b_clear;
}

void CMWRenderBuffer::reset_encode_id()
{
	m_u32_dur_encode = 1;
}
