#ifndef RENDER_H
#define RENDER_H

#include <Windows.h>
#include "d3d11.h"
#include "D3Dcommon.h"
#include "D3DX11.h"
#include "xnamath.h"
#include "D3Dcompiler.h"
#include "MWFOURCC.h"

#define BGR24_CPU

typedef struct _vertex {
	XMFLOAT3 m_pos;
	XMFLOAT2 m_tex;
} s_vertex;

typedef struct _rect {
	int m_cx;
	int m_cy;
	int t_temp;
	int t_temp1;
} s_rect;

typedef struct _csvs {
	unsigned char *m_p_code;
	int m_n_code_len;
} s_csvs;

class MWRender {
public:
	MWRender();
	~MWRender();

public:
	bool initialize(int t_n_width, int t_n_height, DWORD dwFourcc,
			HWND t_hwnd);
	void cleanup_device();

	void paint(unsigned char *data);

protected:
	HRESULT compile_shader_from_file(WCHAR *t_sz_filepath,
					 LPCSTR t_sz_entrypoint,
					 LPCSTR t_sz_shader_model,
					 ID3DBlob **t_pp_blob_out);
	HRESULT compile_shader_from_cs(unsigned char *t_p_data,
				       int t_n_data_len, LPCSTR t_sz_entrypoint,
				       LPCSTR t_sz_shader_model,
				       ID3DBlob **t_pp_blob_out);

	bool create_render_with_fourcc(DWORD t_dw_fourcc);
	void destory_render_with_fourcc(DWORD t_dw_fourcc);

	bool create_render_nv12();

	bool get_vsfs(s_csvs *t_vs, s_csvs *t_ps, DWORD t_dw_fourcc);

protected:
	int m_n_width;
	int m_n_height;

	DWORD m_dw_fourcc;
	int m_n_stride;
	int m_n_size;

	IDXGISwapChain *m_p_swap_chain; //directx swap chain
	DXGI_SWAP_CHAIN_DESC m_sd;      //swap chain description

	ID3D11Device *m_p_device;                //d3d11 device
	ID3D11DeviceContext *m_p_device_context; //device context

	ID3D11Texture2D *m_p_render_buffer;
	ID3D11RenderTargetView *m_p_render_view; //render view

	D3D_FEATURE_LEVEL m_featurelevels;
	D3D_FEATURE_LEVEL m_featurelevel;

	D3D11_VIEWPORT m_vp;

	XMFLOAT3 m_arr_vex[3];
	D3D11_BUFFER_DESC m_vb_bd;
	ID3D11Buffer *m_p_vertex_buffer;
	ID3D11VertexShader *m_p_vertex_shader;

	ID3DBlob *m_p_vs_blob;
	ID3D11InputLayout *m_p_vertex_layout;

	ID3DBlob *m_p_fs_blob;
	ID3D11PixelShader *m_p_fragment_shader;

	ID3D11Texture2D *m_p_texture_2d;
	ID3D11ShaderResourceView *m_p_shader_resource_view; //texture
	ID3D11SamplerState *m_p_sampler_state;
	D3D11_SAMPLER_DESC m_sampler_des;

	ID3D11Texture2D *m_p_texture_2d_2;
	ID3D11ShaderResourceView *m_p_shader_resource_view_2;

	unsigned char *m_p_data;
	unsigned char *m_p_data2;
	int m_n_rc_size;

	ID3D11Buffer *m_d3db_rect;

	bool m_b_set;

private:
};

#endif