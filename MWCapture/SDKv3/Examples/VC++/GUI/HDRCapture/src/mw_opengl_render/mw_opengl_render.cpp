#include "mw_opengl_render.h"
#include "mw_opengl_shader.h"

// rgb range
typedef struct mw_rgb_rng_s {
	double			m_d_min;
	double			m_d_max;
}mw_rgb_rng_t;

typedef struct mw_yuv_rng_s {
	double			m_d_ymin;
	double			m_d_ymax;
	double			m_d_cmax;
	double			m_d_cmid;
}mw_yuv_rng_t;

static const mw_rgb_rng_t g_rgb_rng_limited = {
	16.0/255.0,
	235.0/255.0
};
static const mw_rgb_rng_t g_rgb_rng_full = {
	0.0/255.0,
	255.0/255.0
};

static const mw_yuv_rng_t g_yuv_rng_limited = { 
	16.0 / 255.0, 
	235.0 / 255.0, 
	240.0 / 255.0, 
	128.0 / 255.0 
};

static const mw_yuv_rng_t g_yuv_rng_full = {
	0.0 , 
	255.0 / 255.0, 
	255.0 / 255.0, 
	128.0 / 255.0 
};

static const GLfloat g_af_ver[] = {
	-1.0f, -1.0f,
	1.0f, -1.0f,
	-1.0f,  1.0f,
	1.0f,  1.0f,
};

static const GLfloat g_af_tex[] = {
	0.0f,  0.0f,
	1.0f,  0.0f,
	0.0f,  1.0f,
	1.0f,  1.0f,
};

static const GLfloat g_af_tex_rev[] = {
	0.0f,  1.0f,
	1.0f,  1.0f,
	0.0f,  0.0f,
	1.0f,  0.0f,
};

CMWOpenGLRender::CMWOpenGLRender()
{
	memset(m_a_tex, 0, sizeof(mw_render_tex_t) * 3);
	m_i32_tex_num = 0;
	m_glui_sdr_sh_ver = 0;
	m_glui_sdr_sh_fra = 0;
	m_glui_sdr_pgm = 0;
	m_gli_sdr_loc_ver = 0;
	m_gli_sdr_loc_fra = 0;

	m_gli_pbo = 0;

	m_render_init.m_clr_rng_in = mw_color_range_t::MW_CLR_RNG_CNT;
	m_render_init.m_clr_rng_out = mw_color_range_t::MW_CLR_RNG_CNT;
	m_render_init.m_csp_in = mw_color_space_t::MW_CSP_CNT;
	m_render_init.m_mode = mw_render_fourcc_t::MW_RENDER_FOURCC_CNT;
	m_render_init.m_u32_height = 0;
	m_render_init.m_u32_width = 0;
	m_render_init.m_u8_need_rev = 0;

	memset(m_aglf_rgb_trans, 0, sizeof(GLfloat) * 3);
	memset(m_csp_mat, 0, sizeof(float) * 9);
	memset(m_csp_const, 0, sizeof(float) * 3);


	m_p_render_buffer = NULL;
	m_p_render_buffer = new CMWRenderBuffer();
	m_p_render_buffer->allocate_buffer(4096 * 2160 * 4);

}

CMWOpenGLRender::~CMWOpenGLRender()
{
	clean_up();

	if (m_p_render_buffer != NULL) {
		delete m_p_render_buffer;
		m_p_render_buffer = NULL;
	}
}

void CMWOpenGLRender::clean_up()
{

	if (m_gli_pbo)
	{
		glDeleteBuffers(1, &m_gli_pbo);
		m_gli_pbo = 0;
	}

	// sdr
	if (m_glui_sdr_sh_ver)
	{
		glDeleteShader(m_glui_sdr_sh_ver);
		m_glui_sdr_sh_ver = 0;
	}

	if (m_glui_sdr_sh_fra)
	{
		glDeleteShader(m_glui_sdr_sh_fra);
		m_glui_sdr_sh_fra = 0;
	}

	if (m_glui_sdr_pgm)
	{
		glDeleteProgram(m_glui_sdr_pgm);
		m_glui_sdr_pgm = 0;
	}

	// hdr
	if (m_glui_hdr_sh_ver)
	{
		glDeleteShader(m_glui_hdr_sh_ver);
		m_glui_hdr_sh_ver = 0;
	}

	if (m_glui_hdr_sh_fra)
	{
		glDeleteShader(m_glui_hdr_sh_fra);
		m_glui_hdr_sh_fra = 0;
	}

	if (m_glui_hdr_pgm)
	{
		glDeleteProgram(m_glui_hdr_pgm);
		m_glui_hdr_pgm = 0;
	}

	m_i32_tex_num = 0;
	for (int i = 0;i < 3;i++) {
		if (m_a_tex[i].m_glui_tex != 0) {
			glDeleteTextures(1, &m_a_tex[i].m_glui_tex);
			memset(&m_a_tex[i], 0, sizeof(mw_render_tex_t));
		}
	}
}


void CMWOpenGLRender::close()
{
	clean_up();
}


mw_render_stat_t 
CMWOpenGLRender::open_sdr(mw_render_init_t *rinit)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;
	
	t_stat = open_render(rinit, false);

	return t_stat;
}

int CMWOpenGLRender::render_sdr(
	mw_render_ctrl_t *rctrl, 
	bool black)
{
	void *data = NULL;
	// get render data from render buffer

	cmw_buffer_s* t_p_buffer = NULL;
	if (m_p_render_buffer != NULL) {
		t_p_buffer = m_p_render_buffer->get_buffer_render();
		if (t_p_buffer != NULL)
			data = t_p_buffer->m_p_buffer;
	}

	if (data == NULL)
		black = true;

	switch (m_render_init.m_mode)
	{
	case MW_RENDER_FOURCC_GREY:
		break;
	case MW_RENDER_FOURCC_Y800:
		break;
	case MW_RENDER_FOURCC_Y8:
		break;
	case MW_RENDER_FOURCC_Y16:
		break;
	case MW_RENDER_FOURCC_RGB15:
		break;
	case MW_RENDER_FOURCC_RGB16:
		break;
	case MW_RENDER_FOURCC_RGB24:
		break;
	case MW_RENDER_FOURCC_RGBA:
		render_rgba(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_ARGB:
		render_argb(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_BGR15:
		break;
	case MW_RENDER_FOURCC_BGR16:
		break;
	case MW_RENDER_FOURCC_BGR24:
		break;
	case MW_RENDER_FOURCC_BGRA:
		render_bgra(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_ABGR:
		render_abgr(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_NV16:
		break;
	case MW_RENDER_FOURCC_NV61:
		break;
	case MW_RENDER_FOURCC_I422:
		break;
	case MW_RENDER_FOURCC_YV16:
		break;
	case MW_RENDER_FOURCC_YUY2:
	case MW_RENDER_FOURCC_YUYV:
		render_yuy2(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_UYVY:
		break;
	case MW_RENDER_FOURCC_YVYU:
		break;
	case MW_RENDER_FOURCC_VYUY:
		break;
	case MW_RENDER_FOURCC_I420:
		render_i420(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_IYUV:
		break;
	case MW_RENDER_FOURCC_NV12:
		render_nv12(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_YV12:
		render_yv12(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_NV21:
		render_nv21(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_P010:
		render_p010(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_P210:
		break;
	case MW_RENDER_FOURCC_IYU2:
		break;
	case MW_RENDER_FOURCC_V308:
		break;
	case MW_RENDER_FOURCC_AYUV:
		break;
	case MW_RENDER_FOURCC_UYVA:
		break;
	case MW_RENDER_FOURCC_V408:
		break;
	case MW_RENDER_FOURCC_VYUA:
		break;
	case MW_RENDER_FOURCC_V210:
		break;
	case MW_RENDER_FOURCC_Y410:
		break;
	case MW_RENDER_FOURCC_V410:
		break;
	case MW_RENDER_FOURCC_RGB10:
		break;
	case MW_RENDER_FOURCC_BGR10:
		break;
	case MW_RENDER_FOURCC_CNT:
		break;
	default:
		break;
	}

	if (t_p_buffer != NULL)
		m_p_render_buffer->put_buffer_rended(t_p_buffer);

	return 0;
}

mw_render_stat_t 
CMWOpenGLRender::open_hdr(mw_render_init_t* rinit)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	t_stat = open_render(rinit, true);

	return t_stat;
}

int32_t 
CMWOpenGLRender::render_hdr(
	mw_render_ctrl_ex_t *rctrl, 
	bool black)
{
	void *data = NULL;
	// get render data from render buffer

	cmw_buffer_s* t_p_buffer = NULL;
	if (m_p_render_buffer != NULL) {
		t_p_buffer = m_p_render_buffer->get_buffer_render();
		if (t_p_buffer != NULL)
			data = t_p_buffer->m_p_buffer;
	}

	if (data == NULL)
		black = true;

	switch (m_render_init.m_mode)
	{
	case MW_RENDER_FOURCC_GREY:
		break;
	case MW_RENDER_FOURCC_Y800:
		break;
	case MW_RENDER_FOURCC_Y8:
		break;
	case MW_RENDER_FOURCC_Y16:
		break;
	case MW_RENDER_FOURCC_RGB15:
		break;
	case MW_RENDER_FOURCC_RGB16:
		break;
	case MW_RENDER_FOURCC_RGB24:
		break;
	case MW_RENDER_FOURCC_RGBA:
		break;
	case MW_RENDER_FOURCC_ARGB:
		break;
	case MW_RENDER_FOURCC_BGR15:
		break;
	case MW_RENDER_FOURCC_BGR16:
		break;
	case MW_RENDER_FOURCC_BGR24:
		break;
	case MW_RENDER_FOURCC_BGRA:
		break;
	case MW_RENDER_FOURCC_ABGR:
		break;
	case MW_RENDER_FOURCC_NV16:
		break;
	case MW_RENDER_FOURCC_NV61:
		break;
	case MW_RENDER_FOURCC_I422:
		break;
	case MW_RENDER_FOURCC_YV16:
		break;
	case MW_RENDER_FOURCC_YUY2:
	case MW_RENDER_FOURCC_YUYV:
		break;
	case MW_RENDER_FOURCC_UYVY:
		break;
	case MW_RENDER_FOURCC_YVYU:
		break;
	case MW_RENDER_FOURCC_VYUY:
		break;
	case MW_RENDER_FOURCC_I420:
		break;
	case MW_RENDER_FOURCC_IYUV:
		break;
	case MW_RENDER_FOURCC_NV12:
		break;
	case MW_RENDER_FOURCC_YV12:
		break;
	case MW_RENDER_FOURCC_NV21:
		break;
	case MW_RENDER_FOURCC_P010:
		render_p010_hdr(data, rctrl, black);
		break;
	case MW_RENDER_FOURCC_P210:
		break;
	case MW_RENDER_FOURCC_IYU2:
		break;
	case MW_RENDER_FOURCC_V308:
		break;
	case MW_RENDER_FOURCC_AYUV:
		break;
	case MW_RENDER_FOURCC_UYVA:
		break;
	case MW_RENDER_FOURCC_V408:
		break;
	case MW_RENDER_FOURCC_VYUA:
		break;
	case MW_RENDER_FOURCC_V210:
		break;
	case MW_RENDER_FOURCC_Y410:
		break;
	case MW_RENDER_FOURCC_V410:
		break;
	case MW_RENDER_FOURCC_RGB10:
		break;
	case MW_RENDER_FOURCC_BGR10:
		break;
	case MW_RENDER_FOURCC_CNT:
		break;
	default:
		break;
	}

	if (t_p_buffer != NULL)
		m_p_render_buffer->put_buffer_rended(t_p_buffer);

	return 0;
}

CMWRenderBuffer* CMWOpenGLRender::get_render_buffer()
{
	return m_p_render_buffer;
}

mw_render_stat_t 
CMWOpenGLRender::open_render(
	mw_render_init_t* t_p_rinit, 
	bool t_b_hdr)
{
	m_render_init = *t_p_rinit;
	if (m_render_init.m_mode >= MW_RENDER_FOURCC_CNT)
		return MW_RENDER_STAT_MODE;

	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;
	switch (m_render_init.m_mode)
	{
	case MW_RENDER_FOURCC_GREY:
		break;
	case MW_RENDER_FOURCC_Y800:
		break;
	case MW_RENDER_FOURCC_Y8:
		break;
	case MW_RENDER_FOURCC_Y16:
		break;
	case MW_RENDER_FOURCC_RGB15:
		break;
	case MW_RENDER_FOURCC_RGB16:
		break;
	case MW_RENDER_FOURCC_RGB24:
		break;
	case MW_RENDER_FOURCC_RGBA:
		t_stat = open_rgba(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_ARGB:
		t_stat = open_argb(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_BGR15:
		break;
	case MW_RENDER_FOURCC_BGR16:
		break;
	case MW_RENDER_FOURCC_BGR24:
		break;
	case MW_RENDER_FOURCC_BGRA:
		t_stat = open_bgra(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_ABGR:
		t_stat = open_abgr(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_NV16:
		break;
	case MW_RENDER_FOURCC_NV61:
		break;
	case MW_RENDER_FOURCC_I422:
		break;
	case MW_RENDER_FOURCC_YV16:
		break;
	case MW_RENDER_FOURCC_YUY2:
	case MW_RENDER_FOURCC_YUYV:
		t_stat = open_yuy2(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_UYVY:
		break;
	case MW_RENDER_FOURCC_YVYU:
		break;
	case MW_RENDER_FOURCC_VYUY:
		break;
	case MW_RENDER_FOURCC_I420:
		t_stat = open_i420(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_IYUV:
		break;
	case MW_RENDER_FOURCC_NV12:
		t_stat = open_nv12(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_YV12:
		t_stat = open_yv12(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_NV21:
		t_stat = open_nv21(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_P010:
		t_stat = open_p010(t_b_hdr);
		break;
	case MW_RENDER_FOURCC_P210:
		break;
	case MW_RENDER_FOURCC_IYU2:
		break;
	case MW_RENDER_FOURCC_V308:
		break;
	case MW_RENDER_FOURCC_AYUV:
		break;
	case MW_RENDER_FOURCC_UYVA:
		break;
	case MW_RENDER_FOURCC_V408:
		break;
	case MW_RENDER_FOURCC_VYUA:
		break;
	case MW_RENDER_FOURCC_V210:
		break;
	case MW_RENDER_FOURCC_Y410:
		break;
	case MW_RENDER_FOURCC_V410:
		break;
	case MW_RENDER_FOURCC_RGB10:
		break;
	case MW_RENDER_FOURCC_BGR10:
		break;
	case MW_RENDER_FOURCC_CNT:
		break;
	default:
		break;
	}

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_rgba(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_rgb_trans();

	m_a_tex[0].m_u32_fmt = GL_RGBA;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_inter_fmt = GL_RGBA;
	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(1);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	t_stat = init_render_sdr(
		(char*)g_cs_ver_nm,
		(char*)g_cs_fra_rgba,
		(void*)g_af_ver,
		m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_rgba(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	t_stat = render_uv_rgb(1, rctrl, black);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_argb(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_rgb_trans();

	m_a_tex[0].m_u32_fmt = GL_RGBA;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_inter_fmt = GL_RGBA;
	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(1);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	t_stat = init_render_sdr(
		(char*)g_cs_ver_nm,
		(char*)g_cs_fra_argb,
		(void*)g_af_ver,
		m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_argb(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	t_stat = render_uv_rgb(1, rctrl, black);

	return t_stat;
}

mw_render_stat_t
CMWOpenGLRender::open_bgra(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_rgb_trans();

	m_a_tex[0].m_u32_fmt = GL_RGBA;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_inter_fmt = GL_RGBA;
	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(1);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	t_stat = init_render_sdr(
		(char*)g_cs_ver_nm,
		(char*)g_cs_fra_bgra,
		(void*)g_af_ver,
		m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);

	return t_stat;
}

mw_render_stat_t
CMWOpenGLRender::render_bgra(
	void *data,
	mw_render_ctrl_t *rctrl,
	bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	t_stat = render_uv_rgb(1, rctrl, black);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_abgr(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_rgb_trans();

	m_a_tex[0].m_u32_fmt = GL_RGBA;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_inter_fmt = GL_RGBA;
	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(1);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			(char*)g_cs_ver_nm,
			(char*)g_cs_fra_abgr,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
	}
	

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_abgr(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	t_stat = render_uv_rgb(1, rctrl, black);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_yuy2(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RG;
	m_a_tex[0].m_u32_inter_fmt = GL_RG;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[1].m_u32_fmt = GL_RGBA;
	m_a_tex[1].m_u32_inter_fmt = GL_RGBA;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(2);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			g_cs_ver_nm,
			g_cs_fra_yuyv,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
		if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
			return t_stat;
	}

	t_stat = init_pbo(m_render_init.m_u32_width*m_render_init.m_u32_height * 2);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_yuy2(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	uint32_t t_u32_size = m_render_init.m_u32_width*m_render_init.m_u32_height * 2;
	t_stat = render_uv_yuv_pbo(2, rctrl, black, data, t_u32_size);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_i420(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RED;
	m_a_tex[0].m_u32_inter_fmt = GL_RED;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width/2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height/2;
	m_a_tex[1].m_u32_fmt = GL_RED;
	m_a_tex[1].m_u32_inter_fmt = GL_RED;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[2].m_u32_wid = m_render_init.m_u32_width/2;
	m_a_tex[2].m_u32_hei = m_render_init.m_u32_height/2;
	m_a_tex[2].m_u32_fmt = GL_RED;
	m_a_tex[2].m_u32_inter_fmt = GL_RED;
	m_a_tex[2].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(3);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			g_cs_ver_nm,
			g_cs_fra_i420,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
		if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
			return t_stat;
	}

	t_stat = init_pbo(m_render_init.m_u32_width*m_render_init.m_u32_height * 2);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_i420(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height;
	m_a_tex[2].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height*5/4;
	t_stat = render_uv_yuv(3, rctrl, black);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::open_nv12(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RED;
	m_a_tex[0].m_u32_inter_fmt = GL_RED;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height/ 2;
	m_a_tex[1].m_u32_fmt = GL_RG;
	m_a_tex[1].m_u32_inter_fmt = GL_RG;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(2);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			g_cs_ver_nm,
			g_cs_fra_nv12,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
	}

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_nv12(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;


	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height;
	t_stat = render_uv_yuv(2, rctrl, black);

	return t_stat;
}


mw_render_stat_t CMWOpenGLRender::open_yv12(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RED;
	m_a_tex[0].m_u32_inter_fmt = GL_RED;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height / 2;
	m_a_tex[1].m_u32_fmt = GL_RED;
	m_a_tex[1].m_u32_inter_fmt = GL_RED;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[2].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[2].m_u32_hei = m_render_init.m_u32_height / 2;
	m_a_tex[2].m_u32_fmt = GL_RED;
	m_a_tex[2].m_u32_inter_fmt = GL_RED;
	m_a_tex[2].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(3);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			g_cs_ver_nm,
			g_cs_fra_yv12,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
		if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
			return t_stat;
	}

	t_stat = init_pbo(m_render_init.m_u32_width*m_render_init.m_u32_height * 2);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_yv12(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height;
	m_a_tex[2].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height * 5 / 4;
	t_stat = render_uv_yuv(3, rctrl, black);

	return t_stat;
}


mw_render_stat_t CMWOpenGLRender::open_nv21(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;
	
	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RED;
	m_a_tex[0].m_u32_inter_fmt = GL_RED;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_BYTE;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height / 2;
	m_a_tex[1].m_u32_fmt = GL_RG;
	m_a_tex[1].m_u32_inter_fmt = GL_RG;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_BYTE;

	t_stat = gen_tex(2);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		clean_up();
		t_stat = MW_RENDER_STAT_NOT_SUPPORT;
		return t_stat;
	}
	else {
		t_stat = init_render_sdr(
			g_cs_ver_nm,
			g_cs_fra_nv21,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
	}

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_nv21(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;
	
	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height;
	t_stat = render_uv_yuv(2, rctrl, black);

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::open_p010(bool t_b_hdr)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	calc_yuv_trans();

	m_a_tex[0].m_u32_wid = m_render_init.m_u32_width;
	m_a_tex[0].m_u32_hei = m_render_init.m_u32_height;
	m_a_tex[0].m_u32_fmt = GL_RED;
	m_a_tex[0].m_u32_inter_fmt = GL_RED;
	m_a_tex[0].m_u32_type = GL_UNSIGNED_SHORT;

	m_a_tex[1].m_u32_wid = m_render_init.m_u32_width / 2;
	m_a_tex[1].m_u32_hei = m_render_init.m_u32_height / 2;
	m_a_tex[1].m_u32_fmt = GL_RG;
	m_a_tex[1].m_u32_inter_fmt = GL_RG;
	m_a_tex[1].m_u32_type = GL_UNSIGNED_SHORT;

	t_stat = gen_tex(2);
	if (t_stat != mw_render_stat_t::MW_RENDER_STAT_OK)
		return t_stat;


	t_stat = init_render_sdr(
		g_cs_ver_nm,
		g_cs_fra_nv12,
		(void*)g_af_ver,
		m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex);
	if (t_stat != MW_RENDER_STAT_OK)
		return t_stat;

	if (t_b_hdr) {
		t_stat = init_render_hdr(
			g_cs_ver_nm,
			g_cs_fra_p010,
			(void*)g_af_ver,
			m_render_init.m_u8_need_rev ? (void*)g_af_tex_rev : (void*)g_af_tex
		);
	}

	return t_stat;
}

mw_render_stat_t CMWOpenGLRender::render_p010(void *data, mw_render_ctrl_t *rctrl, bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height*2;
	t_stat = render_uv_yuv(2, rctrl, black);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::render_p010_hdr(
	void* data, 
	mw_render_ctrl_ex_t* rctrl, 
	bool black)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	m_a_tex[0].m_p_data = data;
	m_a_tex[1].m_p_data = (char*)data + m_render_init.m_u32_width*m_render_init.m_u32_height * 2;
	if (rctrl->m_u8_hdr_on)
		t_stat = render_uv_yuv_hdr(2, rctrl, black);
	else
		t_stat = render_uv_yuv(2, &(rctrl->m_ctrl), black);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::init_render_sdr(
	const char*   t_cs_ver,
	const char*	t_cs_frag,
	void*	t_p_ver,
	void*	t_p_tex)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	do 
	{
		m_glui_sdr_sh_ver = glCreateShader(GL_VERTEX_SHADER);
		if (!m_glui_sdr_sh_ver) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHV;
			break;
		}

		m_glui_sdr_sh_fra = glCreateShader(GL_FRAGMENT_SHADER);
		if (!m_glui_sdr_sh_fra) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHF;
			break;
		}

		glShaderSource(m_glui_sdr_sh_ver, 1, (const GLchar**)&t_cs_ver, NULL);
		glShaderSource(m_glui_sdr_sh_fra, 1, (const GLchar**)&t_cs_frag, NULL);

		GLint t_gli_tmp = 0;
		glCompileShader(m_glui_sdr_sh_ver);
		glGetShaderiv(m_glui_sdr_sh_ver, GL_COMPILE_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHV;
			break;
		}
		glCompileShader(m_glui_sdr_sh_fra);
		glGetShaderiv(m_glui_sdr_sh_fra, GL_COMPILE_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			GLchar	t_cs_info[512] = {0};
			GLsizei t_n_size = 0;
			glGetShaderInfoLog(m_glui_sdr_sh_fra, 512, &t_n_size, t_cs_info);
			printf("%s\n", t_cs_info);
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHF;
			break;
		}

		m_glui_sdr_pgm = glCreateProgram();
		if (!m_glui_sdr_pgm) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_PGM;
			break;
		}
		glAttachShader(m_glui_sdr_pgm, m_glui_sdr_sh_ver);
		glAttachShader(m_glui_sdr_pgm, m_glui_sdr_sh_fra);
		glLinkProgram(m_glui_sdr_pgm);
		glGetProgramiv(m_glui_sdr_pgm, GL_LINK_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_PGM;
			break;
		}

		m_gli_sdr_loc_ver = glGetAttribLocation(m_glui_sdr_pgm, "vertexIn");
		m_gli_sdr_loc_fra = glGetAttribLocation(m_glui_sdr_pgm, "textureIn");
		glVertexAttribPointer(m_gli_sdr_loc_ver, 2, GL_FLOAT, 0, 0, t_p_ver);
		glEnableVertexAttribArray(m_gli_sdr_loc_ver);
		glVertexAttribPointer(m_gli_sdr_loc_fra, 2, GL_FLOAT, 0, 0, t_p_tex);
		glEnableVertexAttribArray(m_gli_sdr_loc_fra);

	} while (false);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::init_render_hdr(
	const char* t_cs_ver, 
	const char* t_cs_frag, 
	void* t_p_ver, 
	void* t_p_tex)
{
	mw_render_stat_t t_stat = MW_RENDER_STAT_OK;

	do
	{
		m_glui_hdr_sh_ver = glCreateShader(GL_VERTEX_SHADER);
		if (!m_glui_hdr_sh_ver) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHV;
			break;
		}

		m_glui_hdr_sh_fra = glCreateShader(GL_FRAGMENT_SHADER);
		if (!m_glui_hdr_sh_fra) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHF;
			break;
		}

		glShaderSource(m_glui_hdr_sh_ver, 1, (const GLchar**)&t_cs_ver, NULL);
		glShaderSource(m_glui_hdr_sh_fra, 1, (const GLchar**)&t_cs_frag, NULL);

		GLint t_gli_tmp = 0;
		glCompileShader(m_glui_hdr_sh_ver);
		glGetShaderiv(m_glui_hdr_sh_ver, GL_COMPILE_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHV;
			break;
		}
		glCompileShader(m_glui_hdr_sh_fra);
		glGetShaderiv(m_glui_hdr_sh_fra, GL_COMPILE_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			GLchar	t_cs_info[512] = { 0 };
			GLsizei t_n_size = 0;
			glGetShaderInfoLog(m_glui_hdr_sh_fra, 512, &t_n_size, t_cs_info);
			printf("%s\n", t_cs_info);
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_SHF;
			break;
		}

		m_glui_hdr_pgm = glCreateProgram();
		if (!m_glui_hdr_pgm) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_PGM;
			break;
		}
		glAttachShader(m_glui_hdr_pgm, m_glui_hdr_sh_ver);
		glAttachShader(m_glui_hdr_pgm, m_glui_hdr_sh_fra);
		glLinkProgram(m_glui_hdr_pgm);
		glGetProgramiv(m_glui_hdr_pgm, GL_LINK_STATUS, &t_gli_tmp);
		if (!t_gli_tmp) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_PGM;
			break;
		}

		m_gli_hdr_loc_ver = glGetAttribLocation(m_glui_hdr_pgm, "vertexIn");
		m_gli_hdr_loc_fra = glGetAttribLocation(m_glui_hdr_pgm, "textureIn");
		glVertexAttribPointer(m_gli_hdr_loc_ver, 2, GL_FLOAT, 0, 0, t_p_ver);
		glEnableVertexAttribArray(m_gli_hdr_loc_ver);
		glVertexAttribPointer(m_gli_hdr_loc_fra, 2, GL_FLOAT, 0, 0, t_p_tex);
		glEnableVertexAttribArray(m_gli_hdr_loc_fra);

	} while (false);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::init_pbo(uint32_t t_u32_size)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	glGenBuffers(1, &m_gli_pbo);
	if (!m_gli_pbo) {
		clean_up();
		return mw_render_stat_t::MW_RENDER_STAT_PBO;
	}
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_gli_pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, t_u32_size, 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::gen_tex(int32_t t_i32_tex_num)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	for (int i = 0;i < t_i32_tex_num;i++) {
		glGenTextures(1, &m_a_tex[i].m_glui_tex);
		if (!m_a_tex[i].m_glui_tex) {
			clean_up();
			t_stat = mw_render_stat_t::MW_RENDER_STAT_TEX;
			break;
		}
		glBindTexture(GL_TEXTURE_2D, m_a_tex[i].m_glui_tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			m_a_tex[i].m_u32_inter_fmt,
			m_a_tex[i].m_u32_wid,
			m_a_tex[i].m_u32_hei,
			0,
			m_a_tex[i].m_u32_fmt,
			m_a_tex[i].m_u32_type,
			NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::render_uv_rgb(
	int32_t				t_i32_tex_num,
	mw_render_ctrl_t*	t_p_ctrl,
	bool				t_b_black)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	if (t_b_black) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		return t_stat;
	}

	uint32_t display_w = t_p_ctrl->m_u32_display_w;
	uint32_t display_h = t_p_ctrl->m_u32_display_h;

	glViewport(0, 0, display_w, display_h);
	glUseProgram(m_glui_sdr_pgm);

	for (int i = 0;i < t_i32_tex_num;i++) {
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D, m_a_tex[i].m_glui_tex);
		if (m_a_tex[i].m_p_data) {
			glTexSubImage2D(
				GL_TEXTURE_2D, 
				0, 
				0, 
				0, 
				m_a_tex[i].m_u32_wid,
				m_a_tex[i].m_u32_hei,
				m_a_tex[i].m_u32_fmt, 
				m_a_tex[i].m_u32_type, 
				m_a_tex[i].m_p_data);
		}
	}

	glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_rgb"), 0);
	glUniform3f(glGetUniformLocation(m_glui_sdr_pgm, "rgb_rng"),
		m_aglf_rgb_trans[0],
		m_aglf_rgb_trans[1],
		m_aglf_rgb_trans[2]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::render_uv_yuv(
	int32_t t_i32_tex_num, 
	mw_render_ctrl_t* t_p_ctrl, 
	bool t_b_black)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	if (t_b_black) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		return t_stat;
	}

	uint32_t display_w = t_p_ctrl->m_u32_display_w;
	uint32_t display_h = t_p_ctrl->m_u32_display_h;

	glViewport(0, 0, display_w, display_h);
	glUseProgram(m_glui_sdr_pgm);

	for (int i = 0;i < t_i32_tex_num;i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_a_tex[i].m_glui_tex);
		if (m_a_tex[i].m_p_data) {
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				m_a_tex[i].m_u32_wid,
				m_a_tex[i].m_u32_hei,
				m_a_tex[i].m_u32_fmt,
				m_a_tex[i].m_u32_type,
				m_a_tex[i].m_p_data);
		}
	}

	if (t_i32_tex_num == 1)
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
	else if (t_i32_tex_num == 2) {
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_01"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_01"), 1);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_02"), 2);
	}

	// set uniform
	glUniformMatrix3fv(glGetUniformLocation(m_glui_sdr_pgm, "cspmat"), 1, GL_FALSE, &m_csp_mat[0][0]);
	glUniform3f(glGetUniformLocation(m_glui_sdr_pgm, "cspconst"), m_csp_const[0], m_csp_const[1], m_csp_const[2]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::render_uv_yuv_pbo(
	int32_t t_i32_tex_num, 
	mw_render_ctrl_t* t_p_ctrl, 
	bool t_b_black, 
	void* t_p_data,
	uint32_t t_u32_size)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	if (t_b_black) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		return t_stat;
	}

	uint32_t display_w = t_p_ctrl->m_u32_display_w;
	uint32_t display_h = t_p_ctrl->m_u32_display_h;

	glViewport(0, 0, display_w, display_h);
	glUseProgram(m_glui_sdr_pgm);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_gli_pbo);
	GLubyte* t_p_ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
	if (t_p_ptr) {
		memcpy(t_p_ptr, t_p_data, t_u32_size);
		glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
	}

	for (int i = 0;i < t_i32_tex_num;i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_a_tex[i].m_glui_tex);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0,
			0,
			m_a_tex[i].m_u32_wid,
			m_a_tex[i].m_u32_hei,
			m_a_tex[i].m_u32_fmt,
			m_a_tex[i].m_u32_type,
			NULL);

	}

	if(t_i32_tex_num == 1)
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
	else if (t_i32_tex_num == 2) {
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_01"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_01"), 1);
		glUniform1i(glGetUniformLocation(m_glui_sdr_pgm, "tex_yuv_02"), 2);
	}

	// set uniform
	glUniformMatrix3fv(glGetUniformLocation(m_glui_sdr_pgm, "cspmat"), 1, GL_FALSE, &m_csp_mat[0][0]);
	glUniform3f(glGetUniformLocation(m_glui_sdr_pgm, "cspconst"), m_csp_const[0], m_csp_const[1], m_csp_const[2]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return t_stat;
}

mw_render_stat_t 
CMWOpenGLRender::render_uv_yuv_hdr(
	int32_t t_i32_tex_num, 
	mw_render_ctrl_ex_t* t_p_ctrl, 
	bool t_b_black)
{
	mw_render_stat_t t_stat = mw_render_stat_t::MW_RENDER_STAT_OK;

	if (t_b_black) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		return t_stat;
	}

	uint32_t display_w = t_p_ctrl->m_ctrl.m_u32_display_w;
	uint32_t display_h = t_p_ctrl->m_ctrl.m_u32_display_h;

	glViewport(0, 0, display_w, display_h);
	glUseProgram(m_glui_hdr_pgm);

	for (int i = 0;i < t_i32_tex_num;i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_a_tex[i].m_glui_tex);
		if (m_a_tex[i].m_p_data) {
			glTexSubImage2D(
				GL_TEXTURE_2D,
				0,
				0,
				0,
				m_a_tex[i].m_u32_wid,
				m_a_tex[i].m_u32_hei,
				m_a_tex[i].m_u32_fmt,
				m_a_tex[i].m_u32_type,
				m_a_tex[i].m_p_data);
		}
	}

	if (t_i32_tex_num == 1)
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_00"), 0);
	else if (t_i32_tex_num == 2) {
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_01"), 1);
	}
	else {
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_00"), 0);
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_01"), 1);
		glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "tex_yuv_02"), 2);
	}

	// set uniform
	glUniformMatrix3fv(glGetUniformLocation(m_glui_hdr_pgm, "cspmat"), 1, GL_FALSE, &m_csp_mat[0][0]);
	glUniform3f(glGetUniformLocation(m_glui_hdr_pgm, "cspconst"), m_csp_const[0], m_csp_const[1], m_csp_const[2]);
	glUniform1i(glGetUniformLocation(m_glui_hdr_pgm, "val_ctrl"), t_p_ctrl->m_u8_val_ctrl);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return t_stat;
}

void 
CMWOpenGLRender::calc_rgb_trans()
{
	int t_n_in_min = 0;
	int t_n_in_max = 255;
	int t_n_out_min = 0;
	int t_n_out_max = 255;
	if (m_render_init.m_clr_rng_in == MW_CLR_RNG_LIMITED) {
		t_n_in_min = 16;
		t_n_in_max = 255;
	}
	if (m_render_init.m_clr_rng_out == MW_CLR_RNG_LIMITED) {
		t_n_out_min = 16;
		t_n_out_max = 255;
	}
	m_aglf_rgb_trans[0] = t_n_in_min*1.0 / (t_n_in_max - t_n_in_min);
	m_aglf_rgb_trans[1] = (t_n_out_max - t_n_out_min)*1.0 / (t_n_in_max - t_n_in_min);
	m_aglf_rgb_trans[2] = t_n_out_min*1.0 / 255;
}

void 
CMWOpenGLRender::calc_yuv_trans()
{
	memset(m_csp_mat, 0, sizeof(float) * 9);
	memset(m_csp_const, 0, sizeof(float) * 3);

	float t_f_r = 0.0;
	float t_f_g = 0.0;
	float t_f_b = 0.0;

	switch (m_render_init.m_csp_in)
	{
	case MW_CSP_BT_601:
		t_f_r = 0.299;
		t_f_g = 0.587;
		t_f_b = 0.114;
		break;
	case MW_CSP_BT_709:
	case MW_CSP_AUTO:
		t_f_r = 0.2126;
		t_f_g = 0.7152;
		t_f_b = 0.0722;
		break;
	case MW_CSP_BT_2020:
		t_f_r = 0.2627;
		t_f_g = 0.6780;
		t_f_b = 0.0593;
		break;
	default:
		t_f_r = 0.2126;
		t_f_g = 0.7152;
		t_f_b = 0.0722;
		break;
	}

	m_csp_mat[0][0] = 1;
	m_csp_mat[0][1] = 0;
	m_csp_mat[0][2] = 2*(1-t_f_r);
	m_csp_mat[1][0] = 1;
	m_csp_mat[1][1] = -2 * (1 - t_f_b)*t_f_b / t_f_g;
	m_csp_mat[1][2] = -2 * (1 - t_f_r)*t_f_r / t_f_g;
	m_csp_mat[2][0] = 1;
	m_csp_mat[2][1] = 2 * (1 - t_f_b);
	m_csp_mat[2][2] = 0;

	mw_yuv_rng_t	t_yuv_rng = { 0.0 };
	mw_rgb_rng_t	t_rgb_rng = { 0.0 };

	if (m_render_init.m_clr_rng_in == MW_CLR_RNG_LIMITED)
		t_yuv_rng = g_yuv_rng_limited;
	else
		t_yuv_rng = g_yuv_rng_full;

	if (m_render_init.m_clr_rng_out == MW_CLR_RNG_LIMITED)
		t_rgb_rng = g_rgb_rng_limited;
	else
		t_rgb_rng = g_rgb_rng_full;

	double t_d_ymul = 0.0;
	double t_d_cmul = 0.0;
	t_d_ymul = (t_rgb_rng.m_d_max - t_rgb_rng.m_d_min) / (t_yuv_rng.m_d_ymax - t_yuv_rng.m_d_ymin);
	t_d_cmul = (t_rgb_rng.m_d_max - t_rgb_rng.m_d_min) / (t_yuv_rng.m_d_cmax - t_yuv_rng.m_d_cmid) / 2.0;

	for (int i = 0;i < 3;i++) {
		m_csp_mat[i][0] *= t_d_ymul;
		m_csp_mat[i][1] *= t_d_cmul;
		m_csp_mat[i][2] *= t_d_cmul;
		m_csp_const[i] = t_rgb_rng.m_d_min - m_csp_mat[i][0] * t_yuv_rng.m_d_ymin
			- (m_csp_mat[i][1] + m_csp_mat[i][2])*t_yuv_rng.m_d_cmid;
	}
}


