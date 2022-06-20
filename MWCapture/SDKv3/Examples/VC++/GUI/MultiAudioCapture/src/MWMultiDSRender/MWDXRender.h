/************************************************************************************************/
// MWDXRender.h : interface of the MWDXRender class

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

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)                 \
	{                               \
		if (p) {                \
			(p)->Release(); \
			(p) = NULL;     \
		}                       \
	}
#endif

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

class MWDXRender {
public:
	MWDXRender();
	~MWDXRender();

public:
	bool initialize(int t_n_width, int t_n_height, DWORD dwFourcc,
			bool t_b_reverse, HWND t_hwnd);
	void cleanup_device();

	bool paint(unsigned char *data);

protected:
	HRESULT compile_shader_from_file(WCHAR *t_sz_filepath,
					 LPCSTR t_sz_entrypoint,
					 LPCSTR t_sz_shader_model,
					 ID3DBlob **t_pp_blob_out);
	HRESULT compile_shader_from_cs(unsigned char *t_p_data,
				       int t_n_data_len, LPCSTR t_sz_entrypoint,
				       LPCSTR t_sz_shader_model,
				       ID3DBlob **t_pp_blob_out);

	bool create_render_with_fourcc(DWORD t_dw_fourcc, bool t_b_reverse);
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