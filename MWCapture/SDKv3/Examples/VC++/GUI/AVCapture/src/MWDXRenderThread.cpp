#include "stdafx.h"
#include "MWDXRenderThread.h"

CMWDXRenderThread::CMWDXRenderThread()
{
	m_p_render = NULL;
	m_p_buf = NULL;

	m_h_thread_render = NULL;
	m_h_event_exit = NULL;
	m_h_event_render = NULL;

	m_p_render = new MWDXRender();

	m_b_running = false;
}

CMWDXRenderThread::~CMWDXRenderThread()
{
	close();

	if (m_p_render != NULL) {
		delete m_p_render;
		m_p_render = NULL;
	}
}

bool CMWDXRenderThread::open_render(mw_render_init_t *t_p_init, HWND t_h_wnd)
{
	bool t_b_ret = false;
	t_b_ret = m_p_render->initialize(
		t_p_init->m_u32_width, t_p_init->m_u32_height, t_p_init->m_mode,
		t_p_init->m_u8_need_rev, t_h_wnd, t_p_init->m_csp_in,
		t_p_init->m_clr_rng_in, t_p_init->m_clr_rng_out);
	if (!t_b_ret)
		return t_b_ret;

	m_h_event_exit = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_h_event_render = CreateEvent(NULL, FALSE, FALSE, NULL);

	m_h_thread_render = CreateThread(NULL, 0, render_thread, this, 0, NULL);
	if (m_h_thread_render == NULL) {
		close();
		t_b_ret = false;
	} else
		t_b_ret = true;

	return t_b_ret;
}

void CMWDXRenderThread::render()
{
	if (m_b_running && m_h_event_render) {
		SetEvent(m_h_event_render);
	}
}

void CMWDXRenderThread::close()
{
	m_b_running = false;
	if (m_h_event_exit) {
		SetEvent(m_h_event_exit);
		WaitForSingleObject(m_h_thread_render, INFINITE);
		CloseHandle(m_h_event_exit);
		m_h_event_exit = NULL;
		CloseHandle(m_h_thread_render);
		m_h_thread_render = NULL;
	}
	if (m_h_event_render) {
		CloseHandle(m_h_event_render);
		m_h_event_render = NULL;
	}

	m_p_render->cleanup_device();
}

void CMWDXRenderThread::set_render_buffer(CMWRenderBuffer *t_p_buf)
{
	m_p_buf = t_p_buf;
}

DWORD CMWDXRenderThread::render_thread_proc()
{
	m_b_running = true;

	HANDLE t_a_events[] = {m_h_event_exit, m_h_event_render};
	DWORD t_dw_ret = 0;

	uint32_t t_u32_id = -1;
	uint32_t t_u32_id_render = -1;
	while (m_b_running) {
		mw_buffer_t *t_p_buf = m_p_buf->get_buf_render(t_u32_id);
		if (t_u32_id == t_u32_id_render)
			continue;
		else {
			t_u32_id_render = t_u32_id;
			if (t_p_buf != NULL) {
				m_p_render->paint(
					(unsigned char *)t_p_buf->m_p_buf);
			}
		}
	}

	return 0;
}
