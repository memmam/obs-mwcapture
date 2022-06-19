#ifndef MWDXRENDERTHREAD_H
#define MWDXRENDERTHREAD_H

#include "MWRenderBuffer.h"
#include "MWDXRender.h"

typedef struct mw_render_init_s
{
	uint32_t					m_mode;
	uint32_t					m_u32_width;
	uint32_t					m_u32_height;
	uint8_t						m_u8_need_rev;
	mw_color_space_level_e		m_clr_rng_in;
	mw_color_space_level_e		m_clr_rng_out;
	mw_color_space_e			m_csp_in;
}mw_render_init_t;

typedef struct mw_render_ctrl_s
{
	uint32_t m_u32_display_w;
	uint32_t m_u32_display_h;
}mw_render_ctrl_t;

class CMWDXRenderThread{
public:
	CMWDXRenderThread();
	~CMWDXRenderThread();

public:
	bool open_render(mw_render_init_t* t_p_init,HWND t_h_wnd);
	void render();
	void close();

	void set_render_buffer(CMWRenderBuffer* t_p_buf);

public:
	static DWORD WINAPI render_thread(LPVOID t_p_param)
	{
		CMWDXRenderThread* t_p_this = (CMWDXRenderThread*)t_p_param;
		if(t_p_this)
			return t_p_this->render_thread_proc();

		return 0;
	}

protected:
	DWORD render_thread_proc();

protected:
	MWDXRender*			m_p_render;
	CMWRenderBuffer*	m_p_buf;

	HANDLE				m_h_thread_render;
	HANDLE				m_h_event_exit;
	HANDLE				m_h_event_render;
	bool				m_b_running;
};

#endif