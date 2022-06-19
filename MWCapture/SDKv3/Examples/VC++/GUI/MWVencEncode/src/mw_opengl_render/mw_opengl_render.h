#ifndef MW_OPENGL_RENDER_H
#define MW_OPENGL_RENDER_H

#define  GLFW_IMGUI
#include <stdint.h>

typedef enum mw_render_fourcc {
	MW_RENDER_FOURCC_GREY,
	MW_RENDER_FOURCC_Y800,
	MW_RENDER_FOURCC_Y8,
	MW_RENDER_FOURCC_Y16,
	MW_RENDER_FOURCC_RGB15,
	MW_RENDER_FOURCC_RGB16,
	MW_RENDER_FOURCC_RGB24,
	MW_RENDER_FOURCC_RGBA,
	MW_RENDER_FOURCC_ARGB,
	MW_RENDER_FOURCC_BGR15,
	MW_RENDER_FOURCC_BGR16,
	MW_RENDER_FOURCC_BGR24,
	MW_RENDER_FOURCC_BGRA,
	MW_RENDER_FOURCC_ABGR,
	MW_RENDER_FOURCC_NV16,
	MW_RENDER_FOURCC_NV61,
	MW_RENDER_FOURCC_I422,
	MW_RENDER_FOURCC_YV16,
	MW_RENDER_FOURCC_YUY2,
	MW_RENDER_FOURCC_YUYV,
	MW_RENDER_FOURCC_UYVY,
	MW_RENDER_FOURCC_YVYU,
	MW_RENDER_FOURCC_VYUY,
	MW_RENDER_FOURCC_I420,
	MW_RENDER_FOURCC_IYUV,
	MW_RENDER_FOURCC_NV12,
	MW_RENDER_FOURCC_YV12,
	MW_RENDER_FOURCC_NV21,
	MW_RENDER_FOURCC_P010,
	MW_RENDER_FOURCC_P210,
	MW_RENDER_FOURCC_IYU2,
	MW_RENDER_FOURCC_V308,
	MW_RENDER_FOURCC_AYUV,
	MW_RENDER_FOURCC_UYVA,
	MW_RENDER_FOURCC_V408,
	MW_RENDER_FOURCC_VYUA,
	MW_RENDER_FOURCC_V210,
	MW_RENDER_FOURCC_Y410,
	MW_RENDER_FOURCC_V410,
	MW_RENDER_FOURCC_RGB10,
	MW_RENDER_FOURCC_BGR10,
	MW_RENDER_FOURCC_CNT
}mw_render_fourcc_t;

#ifdef GLFW_IMGUI  
#include "GL/gl3w.h"
#include <GLFW/glfw3.h>
#else  
#include <GL/glew.h> 
#if defined(_MSC_VER)
//#define _AFXDLL
//#include <afxwin.h>
#elif defined(__GNUC__) && !defined(__arm__)	
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>
#endif
#endif	

#include "mw_render_buffer.h"

typedef enum mw_color_range_e{
	MW_CLR_RNG_LIMITED,
	MW_CLR_RNG_FULL,
	MW_CLR_RNG_CNT
}mw_color_range_t;

typedef enum mw_color_space_e {
	MW_CSP_AUTO,
	MW_CSP_BT_601,
	MW_CSP_BT_709,
	MW_CSP_BT_2020,
	MW_CSP_CNT
}mw_color_space_t;

typedef struct mw_render_ctrl_s
{
	uint32_t m_u32_display_w;
	uint32_t m_u32_display_h;
}mw_render_ctrl_t;

typedef struct mw_render_ctrl_ex_s {
	mw_render_ctrl_t	m_ctrl;
	uint8_t				m_u8_hdr_on;
	uint8_t				m_u8_val_ctrl;
	uint8_t				m_u8_threshold;
}mw_render_ctrl_ex_t;

typedef struct mw_render_init_s
{
	mw_render_fourcc_t		m_mode;
	uint32_t				m_u32_width;
	uint32_t				m_u32_height;
	uint8_t					m_u8_need_rev;
	mw_color_range_t		m_clr_rng_in;
	mw_color_range_t		m_clr_rng_out;
	mw_color_space_t		m_csp_in;
}mw_render_init_t;

typedef enum mw_render_stat_e {
	MW_RENDER_STAT_OK = 0,
	MW_RENDER_STAT_TEX,
	MW_RENDER_STAT_SHV,
	MW_RENDER_STAT_SHF,
	MW_RENDER_STAT_PGM,
	MW_RENDER_STAT_MODE,
	MW_RENDER_STAT_PBO,
	MW_RENDER_STAT_NOT_SUPPORT,
	MW_RENDER_STAT_CNT
}mw_render_stat_t;

typedef struct mw_render_tex_s {
	GLuint			m_glui_tex;
	uint32_t		m_u32_wid;
	uint32_t		m_u32_hei;
	uint32_t		m_u32_inter_fmt;
	uint32_t		m_u32_fmt;
	uint32_t		m_u32_type;
	void*			m_p_data;
}mw_render_tex_t;

class CMWOpenGLRender
{
public:
	CMWOpenGLRender();
	~CMWOpenGLRender();

public:
	mw_render_stat_t open_sdr(mw_render_init_t* rinit);
	int32_t render_sdr(mw_render_ctrl_t *rctrl, bool black);

	mw_render_stat_t open_hdr(mw_render_init_t* rinit);
	int32_t render_hdr(mw_render_ctrl_ex_t *rctrl, bool black);

	void close();

	CMWRenderBuffer* get_render_buffer();
protected:
	mw_render_stat_t open_render(mw_render_init_t* t_p_rinit,bool t_b_hdr);

	//MW_RENDER_FOURCC_GREY
	//MW_RENDER_FOURCC_Y800
	//MW_RENDER_FOURCC_Y8
	//MW_RENDER_FOURCC_Y16
	//MW_RENDER_FOURCC_RGB15
	//MW_RENDER_FOURCC_RGB16
	//MW_RENDER_FOURCC_RGB24
	//MW_RENDER_FOURCC_RGBA
	mw_render_stat_t open_rgba(bool t_b_hdr);
	mw_render_stat_t render_rgba(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_ARGB
	mw_render_stat_t open_argb(bool t_b_hdr);
	mw_render_stat_t render_argb(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_BGR15
	//MW_RENDER_FOURCC_BGR16
	//MW_RENDER_FOURCC_BGR24
	//MW_RENDER_FOURCC_BGRA
	mw_render_stat_t open_bgra(bool t_b_hdr);
	mw_render_stat_t render_bgra(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_ABGR
	mw_render_stat_t open_abgr(bool t_b_hdr);
	mw_render_stat_t render_abgr(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_NV16
	//MW_RENDER_FOURCC_NV61
	//MW_RENDER_FOURCC_I422
	//MW_RENDER_FOURCC_YV16

	//MW_RENDER_FOURCC_YUY2
	//MW_RENDER_FOURCC_YUYV
	mw_render_stat_t open_yuy2(bool t_b_hdr);
	mw_render_stat_t render_yuy2(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_UYVY
	//MW_RENDER_FOURCC_YVYU
	//MW_RENDER_FOURCC_VYUY
	//MW_RENDER_FOURCC_I420
	mw_render_stat_t open_i420(bool t_b_hdr);
	mw_render_stat_t render_i420(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_IYUV
	//MW_RENDER_FOURCC_NV12
	mw_render_stat_t open_nv12(bool t_b_hdr);
	mw_render_stat_t render_nv12(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_YV12
	mw_render_stat_t open_yv12(bool t_b_hdr);
	mw_render_stat_t render_yv12(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_NV21
	mw_render_stat_t open_nv21(bool t_b_hdr);
	mw_render_stat_t render_nv21(void *data, mw_render_ctrl_t *rctrl, bool black);

	//MW_RENDER_FOURCC_P010
	mw_render_stat_t open_p010(bool t_b_hdr);
	mw_render_stat_t render_p010(void *data, mw_render_ctrl_t *rctrl, bool black);
	mw_render_stat_t render_p010_hdr(void *data, mw_render_ctrl_ex_t *rctrl, bool black);

	//MW_RENDER_FOURCC_P210
	//MW_RENDER_FOURCC_IYU2
	//MW_RENDER_FOURCC_V308
	//MW_RENDER_FOURCC_AYUV
	//MW_RENDER_FOURCC_UYVA
	//MW_RENDER_FOURCC_V408
	//MW_RENDER_FOURCC_VYUA
	//MW_RENDER_FOURCC_V210
	//MW_RENDER_FOURCC_Y410
	//MW_RENDER_FOURCC_V410
	//MW_RENDER_FOURCC_RGB10
	//MW_RENDER_FOURCC_BGR10

 
	mw_render_stat_t init_render_sdr(
		const char*		t_cs_ver,
		const char*		t_cs_frag,
		void*			t_p_ver,
		void*			t_p_tex
	);

	mw_render_stat_t init_render_hdr(
		const char*		t_cs_ver,
		const char*		t_cs_frag,
		void*			t_p_ver,
		void*			t_p_tex
	);

	mw_render_stat_t init_pbo(
		uint32_t t_u32_size
	);

	mw_render_stat_t gen_tex(
		int32_t t_i32_tex_num
	);

	mw_render_stat_t render_uv_rgb(
		int32_t				t_i32_tex_num,
		mw_render_ctrl_t*	t_p_ctrl,
		bool				t_b_black
	);

	mw_render_stat_t render_uv_yuv(
		int32_t				t_i32_tex_num,
		mw_render_ctrl_t*	t_p_ctrl,
		bool				t_b_black
	);

	mw_render_stat_t render_uv_yuv_pbo(
		int32_t				t_i32_tex_num,
		mw_render_ctrl_t*	t_p_ctrl,
		bool				t_b_black,
		void*				t_p_data,
		uint32_t			t_u32_size
	);

	mw_render_stat_t render_uv_yuv_hdr(
		int32_t					t_i32_tex_num,
		mw_render_ctrl_ex_t*	t_p_ctrl,
		bool					t_b_black
	);

	// for rgb trans rgb
	void calc_rgb_trans();

	// for yuv trans rgb
	void calc_yuv_trans();

private:
	void clean_up();

private:
	// rgb range
	GLfloat					m_aglf_rgb_trans[3];

	// yuv
	float					m_csp_mat[3][3];
	float					m_csp_const[3];

	mw_render_init_t		m_render_init;
	mw_render_tex_t			m_a_tex[3];
	int32_t					m_i32_tex_num;

	// buffer
	CMWRenderBuffer*		m_p_render_buffer;

	// pbo
	GLuint					m_gli_pbo;

	// sdr
	GLuint					m_glui_sdr_sh_ver;
	GLuint					m_glui_sdr_sh_fra;
	GLuint					m_glui_sdr_pgm;
	GLint					m_gli_sdr_loc_ver;
	GLint					m_gli_sdr_loc_fra;

	// hdr
	GLuint					m_glui_hdr_pgm;
	GLuint					m_glui_hdr_sh_ver;
	GLuint					m_glui_hdr_sh_fra;
	GLuint					m_gli_hdr_loc_ver;
	GLuint					m_gli_hdr_loc_fra;
};


#endif
