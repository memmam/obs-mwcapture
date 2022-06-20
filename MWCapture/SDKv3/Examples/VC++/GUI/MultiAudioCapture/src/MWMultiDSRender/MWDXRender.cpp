/************************************************************************************************/
// MWDXRender.cpp : implementation of the MWDXRender class

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
#include "MWDXRender.h"
#include <Windows.h>
#include <WinGDI.h>
#include <d3d9.h>

unsigned char g_cs_vs_yuy2[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";

unsigned char g_cs_ps_yuy2[] = "struct VS_INPUT \
{ \
	float4 Pos : POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
struct PS_INPUT \
{ \
	float4 Pos : SV_POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
cbuffer cbNeverChanges : register(b0) \
{ \
	int m_cx; \
	int m_cy; \
	int t_temp; \
	int t_temp2; \
}; \
Texture2D txDiffuse; \
SamplerState samLinear; \
float4 PS(PS_INPUT input) : SV_Target \
{ \
	int t_n_y = m_cy * input.Tex.y; \
	int t_f_x = floor(m_cx *input.Tex.x); \
	int t_n_index = floor(t_f_x) % 2; \
	if (t_n_index == 0) { \
		float t_f_next = input.Tex.x + (1.0 / m_cx); \
		float2 t_tex; \
		t_tex.x = t_f_next; \
		t_tex.y = input.Tex.y; \
		float y = txDiffuse.Sample(samLinear, input.Tex).x; \
		float u = txDiffuse.Sample(samLinear, input.Tex).y; \
		float v = txDiffuse.Sample(samLinear, t_tex).y; \
		y = 1.1643*(y - 0.0625); \
		u = u - 0.5; \
		v = v - 0.5; \
		float r = y + 1.5958*v; \
		float g = y - 0.39173*u - 0.81290*v; \
		float b = y + 2.017*u; \
		return float4(r, g, b, 1.0f); \
	} \
	else if (t_n_index == 1) { \
		float t_f_next = input.Tex.x - (1.0 / m_cx); \
		float2 t_tex; \
		t_tex.x = t_f_next; \
		t_tex.y = input.Tex.y; \
		float y = txDiffuse.Sample(samLinear, input.Tex).x; \
		float u = txDiffuse.Sample(samLinear, t_tex).y; \
		float v = txDiffuse.Sample(samLinear, input.Tex).y; \
		y = 1.1643*(y - 0.0625); \
		u = u - 0.5; \
		v = v - 0.5; \
		float r = y + 1.5958*v; \
		float g = y - 0.39173*u - 0.81290*v; \
		float b = y + 2.017*u; \
		return float4(r, g, b, 1.0f); \
	} \
}";

unsigned char g_cs_vs_bgra[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";

unsigned char g_cs_ps_bgra[] = "struct VS_INPUT \
{ \
	float4 Pos : POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
struct PS_INPUT \
{ \
	float4 Pos : SV_POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
Texture2D txDiffuse; \
SamplerState samLinear; \
float4 PS(PS_INPUT input) : SV_Target \
{ \
	float b= txDiffuse.Sample(samLinear, input.Tex).x; \
	float g= txDiffuse.Sample(samLinear, input.Tex).y; \
	float r= txDiffuse.Sample(samLinear, input.Tex).z; \
	float a= txDiffuse.Sample(samLinear, input.Tex).w; \
	return float4(r,g,b,a); \
}";

unsigned char g_cs_vs_rgba[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";

unsigned char g_cs_ps_rgba[] = "struct VS_INPUT \
{ \
	float4 Pos : POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
struct PS_INPUT \
{ \
	float4 Pos : SV_POSITION; \
	float2 Tex : TEXCOORD0; \
	}; \
Texture2D txDiffuse; \
SamplerState samLinear; \
float4 PS(PS_INPUT input) : SV_Target \
{ \
	float r= txDiffuse.Sample(samLinear, input.Tex).x; \
	float g= txDiffuse.Sample(samLinear, input.Tex).y; \
	float b= txDiffuse.Sample(samLinear, input.Tex).z; \
	float a= txDiffuse.Sample(samLinear, input.Tex).w; \
	return float4(r,g,b,a); \
}";

#ifndef BGR24_CPU
unsigned char g_cs_vs_bgr24[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";
unsigned char g_cs_ps_bgr24[] = "struct VS_INPUT\n \
{\n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT\n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
cbuffer cbNeverChanges : register(b0)\n \
{\n \
int m_cx;\n \
int m_cy;\n \
int t_temp;\n \
int t_temp2;\n \
};\n \
Texture2D txDiffuse;\n \
SamplerState samLinear;\n \
float4 PS(PS_INPUT input) : SV_Target\n \
{\n \
	int t_n_x=input.Tex.x*m_cx;\n \
	int t_n_y=input.Tex.y*m_cy;\n \
	int t_n_index=t_n_y*m_cx+t_n_x;\n \
	int t_n_t=(t_n_index*3.0)/(m_cx*4.0);\n \
	float t_f_t=(t_n_t*1.0)/m_cy;\n \
	int t_n_sx=(t_n_index*3)%(m_cx*4);\n \
	int t_n_sdx=(t_n_sx)%4;\n \
	float t_f_s=(t_n_sx)/(1.0*m_cx);\n \
	float r=0.0f;\n \
	float g=0.0f;\n \
	float b=0.0f;\n \
	float a=1.0f;\n \
	float2 t_tex;\n \
	float2 t_tex2;\n \
	if(t_n_sdx==0){\n \
		t_tex.x = t_f_s;\n \
		t_tex.y = t_f_t;\n \
		r=txDiffuse.Sample(samLinear, t_tex).x;\n \
		g=txDiffuse.Sample(samLinear, t_tex).y;\n \
		b=txDiffuse.Sample(samLinear, t_tex).z;\n \
		if(t_n_y==0&&t_n_sx<960)\n \
		{\n \
			return float4(1.0,0.0,0.0,1.0);\n \
		}\n \
	}\n \
	else if(t_n_sdx==1){\n \
		t_tex.x = t_f_s;\n \
		t_tex.y = t_f_t;\n \
		r=txDiffuse.Sample(samLinear, t_tex).y;\n \
		g=txDiffuse.Sample(samLinear, t_tex).z;\n \
		b=txDiffuse.Sample(samLinear, t_tex).w;\n \
	}\n \
	else if(t_n_sdx==2){\n \
		t_tex.x = t_f_s;\n \
		t_tex.y = t_f_t;\n \
		r=txDiffuse.Sample(samLinear, t_tex).z;\n \
		g=txDiffuse.Sample(samLinear, t_tex).w;\n \
		if(t_n_sx>=(m_cx*4-4)){\n \
			t_tex2.x=0.0f;\n \
			t_tex2.y=t_f_t+(1.0/m_cy);\n \
		}\n \
		else{\n \
			t_tex2.x=t_f_s+(1.0/m_cx);\n \
			t_tex2.y=t_f_t;\n \
		}\n \
		b=txDiffuse.Sample(samLinear, t_tex2).x;\n \
	}\n \
	else if(t_n_sdx==3){\n \
		t_tex.x = t_f_s;\n \
		t_tex.y = t_f_t;\n \
		r=txDiffuse.Sample(samLinear, t_tex).w;\n \
		if(t_n_sx>=(m_cx*4-4)){\n \
			t_tex2.x=0.0f;\n \
			t_tex2.y=t_f_t+(1.0/m_cy);\n \
		}\n \
		else{\n \
			t_tex2.x=t_f_s+(1.0/m_cx);\n \
			t_tex2.y=t_f_t;\n \
		}\n \
		g=txDiffuse.Sample(samLinear, t_tex2).x;\n \
		b=txDiffuse.Sample(samLinear, t_tex2).y;\n \
	} \
	return float4(r,g,b,a); \
}";
#else
unsigned char g_cs_vs_bgr24[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";

unsigned char g_cs_ps_bgr24[] = "struct VS_INPUT \
{ \
	float4 Pos : POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
struct PS_INPUT \
{ \
	float4 Pos : SV_POSITION; \
	float2 Tex : TEXCOORD0; \
}; \
Texture2D txDiffuse; \
SamplerState samLinear; \
float4 PS(PS_INPUT input) : SV_Target \
{ \
	float b= txDiffuse.Sample(samLinear, input.Tex).x; \
	float g= txDiffuse.Sample(samLinear, input.Tex).y; \
	float r= txDiffuse.Sample(samLinear, input.Tex).z; \
	float a= txDiffuse.Sample(samLinear, input.Tex).w; \
	return float4(r,g,b,a); \
}";

#endif

unsigned char g_cs_vs_uyvy[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";
unsigned char g_cs_ps_uyvy[] = "struct VS_INPUT\n \
{\n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT\n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
cbuffer cbNeverChanges : register(b0)\n \
{\n \
	int m_cx;\n \
	int m_cy;\n \
	int t_temp;\n \
	int t_temp2;\n \
};\n \
Texture2D txDiffuse;\n \
SamplerState samLinear;\n \
float4 PS(PS_INPUT input) : SV_Target\n \
{\n \
	int t_n_y = m_cy * input.Tex.y;\n \
	int t_f_x = floor(m_cx *input.Tex.x);\n \
	int t_n_index = floor(t_f_x) % 2;\n \
	if (t_n_index == 0) {\n \
		float t_f_next = input.Tex.x + (1.0 / m_cx);\n \
		float2 t_tex;\n \
		t_tex.x = t_f_next;\n \
		t_tex.y = input.Tex.y;\n \
		float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
		float u = txDiffuse.Sample(samLinear, input.Tex).x;\n \
		float v = txDiffuse.Sample(samLinear, t_tex).x;\n \
		y = 1.1643*(y - 0.0625);\n \
		u = u - 0.5;\n \
		v = v - 0.5;\n \
		float r = y + 1.5958*v;\n \
		float g = y - 0.39173*u - 0.81290*v;\n \
		float b = y + 2.017*u;\n \
		return float4(r, g, b, 1.0f);\n \
	}\n \
	else if (t_n_index == 1) {\n \
		float t_f_next = input.Tex.x - (1.0 / m_cx);\n \
		float2 t_tex;\n \
		t_tex.x = t_f_next;\n \
		t_tex.y = input.Tex.y;\n \
		float y = txDiffuse.Sample(samLinear, input.Tex).y;\n \
		float u = txDiffuse.Sample(samLinear, t_tex).x;\n \
		float v = txDiffuse.Sample(samLinear, input.Tex).x;\n \
		y = 1.1643*(y - 0.0625);\n \
		u = u - 0.5;\n \
		v = v - 0.5;\n \
		float r = y + 1.5958*v;\n \
		float g = y - 0.39173*u - 0.81290*v;\n \
		float b = y + 2.017*u;\n \
		return float4(r, g, b, 1.0f);\n \
	}\n \
}\n";

unsigned char g_cs_vs_nv12[] = "struct VS_INPUT\n \
{ \n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT \n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
PS_INPUT VS(VS_INPUT input)\n \
{\n \
	PS_INPUT output;\n \
	output.Pos = input.Pos;\n \
	output.Tex = input.Tex;\n \
	return output;\n \
}";

/**/
unsigned char g_cs_ps_nv12[] = "struct VS_INPUT\n \
{\n \
	float4 Pos : POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
struct PS_INPUT\n \
{\n \
	float4 Pos : SV_POSITION;\n \
	float2 Tex : TEXCOORD0;\n \
};\n \
cbuffer cbNeverChanges : register(b0)\n \
{\n \
	int m_cx;\n \
	int m_cy;\n \
	int t_temp;\n \
	int t_temp2;\n \
};\n \
Texture2D txtYregister: register(t0);\n \
Texture2D txtUVregister:register(t1);\n \
SamplerState samLinear;\n \
float4 PS(PS_INPUT input) : SV_Target\n \
{\n \
	int t_n_nx=input.Tex.x*m_cx;\n \
	int t_n_ny=input.Tex.y*m_cy;\n \
	float t_f_x=floor(t_n_nx/2)/(m_cx/2);\n \
	float t_f_y = floor(t_n_ny/2)/(m_cy/2);\n \
	float2 t_tex;\n \
	t_tex.x = t_f_x;\n \
	t_tex.y = t_f_y;\n \
	float y=txtYregister.Sample(samLinear, input.Tex).x;\n \
	float u=0.0f;\n \
	float v=0.0f;\n \
	u=txtUVregister.Sample(samLinear, t_tex).x;\n \
	v=txtUVregister.Sample(samLinear, t_tex).y;\n \
	y = 1.1643*(y - 0.0625);\n \
	u = u - 0.5;\n \
	v = v - 0.5;\n \
	float r = y + 1.5958*v;\n \
	float g = y - 0.39173*u - 0.81290*v;\n \
	float b = y + 2.017*u;\n \
	return float4(r, g, b, 1.0f);\n \
}\n";

bool MWDXRender::get_vsfs(s_csvs *t_vs, s_csvs *t_ps, DWORD t_dw_fourcc)
{
	bool t_ret = false;

	switch (t_dw_fourcc) {
	case MWFOURCC_RGBA:
		t_vs->m_p_code = g_cs_vs_rgba;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgba);
		t_ps->m_p_code = g_cs_ps_rgba;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgba);
		t_ret = true;
		break;
	case MWFOURCC_BGRA:
		t_vs->m_p_code = g_cs_vs_bgra;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgra);
		t_ps->m_p_code = g_cs_ps_bgra;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgra);
		t_ret = true;
		break;
	case MWFOURCC_YUY2:
		t_vs->m_p_code = g_cs_vs_yuy2;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yuy2);
		t_ps->m_p_code = g_cs_ps_yuy2;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yuy2);
		t_ret = true;
		break;
	case MWFOURCC_BGR24:
		t_vs->m_p_code = g_cs_vs_bgr24;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgr24);
		t_ps->m_p_code = g_cs_ps_bgr24;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgr24);
		break;
	case MWFOURCC_UYVY:
		t_vs->m_p_code = g_cs_vs_uyvy;
		t_vs->m_n_code_len = sizeof(g_cs_vs_uyvy);
		t_ps->m_p_code = g_cs_ps_uyvy;
		t_ps->m_n_code_len = sizeof(g_cs_ps_uyvy);
		break;
	case MWFOURCC_NV12:
		t_vs->m_p_code = g_cs_vs_nv12;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv12);
		t_ps->m_p_code = g_cs_ps_nv12;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv12);
		break;
	default:
		t_vs->m_p_code = g_cs_vs_bgra;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgra);
		t_ps->m_p_code = g_cs_ps_bgra;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgra);
		t_ret = false;
		break;
	}

	return t_ret;
}

MWDXRender::MWDXRender()
{
	m_p_data = NULL;
	m_p_data2 = NULL;
	m_p_vertex_buffer = NULL;
	m_p_vertex_layout = NULL;
	m_p_vertex_shader = NULL;
	m_p_sampler_state = NULL;
	m_d3db_rect = NULL;
	m_p_texture_2d = NULL;
	m_p_shader_resource_view = NULL;
	m_p_texture_2d_2 = NULL;
	m_p_shader_resource_view_2 = NULL;
	m_p_fragment_shader = NULL;
	m_p_render_view = NULL;
	m_p_swap_chain = NULL;
	m_p_device_context = NULL;
	m_p_device = NULL;
}

MWDXRender::~MWDXRender() {}

HRESULT MWDXRender::compile_shader_from_file(WCHAR *t_sz_filepath,
					     LPCSTR t_sz_entrypoint,
					     LPCSTR t_sz_shader_model,
					     ID3DBlob **t_pp_blob_out)
{
	HRESULT t_hr = S_FALSE;

	DWORD t_dw_shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr = D3DX11CompileFromFile(t_sz_filepath, NULL, NULL, t_sz_entrypoint,
				     t_sz_shader_model, t_dw_shaderflags, 0,
				     NULL, t_pp_blob_out, &t_p_errorblod, NULL);
	if (FAILED(t_hr)) {
		if (t_p_errorblod != NULL) {
			//output the err
			//release
			//TRACE("%s\n",t_p_errorblod->GetBufferPointer());
			t_p_errorblod->Release();
			t_p_errorblod = NULL;
		}
	}
	if (t_p_errorblod) {
		t_p_errorblod->Release();
		t_p_errorblod = NULL;
	}
	return t_hr;
}

HRESULT MWDXRender::compile_shader_from_cs(unsigned char *t_p_data,
					   int t_n_data_len,
					   LPCSTR t_sz_entrypoint,
					   LPCSTR t_sz_shader_model,
					   ID3DBlob **t_pp_blob_out)
{
	HRESULT t_hr = S_FALSE;

	DWORD t_dw_shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr = D3DCompile(t_p_data, t_n_data_len, NULL, NULL, NULL,
			  t_sz_entrypoint, t_sz_shader_model, t_dw_shaderflags,
			  0, t_pp_blob_out, &t_p_errorblod);
	if (FAILED(t_hr)) {
		if (t_p_errorblod != NULL) {
			//output the err
			//release
			//TRACE("%s\n", t_p_errorblod->GetBufferPointer());
			t_p_errorblod->Release();
			t_p_errorblod = NULL;
		}
	}
	if (t_p_errorblod) {
		t_p_errorblod->Release();
		t_p_errorblod = NULL;
	}
	return t_hr;
}

bool MWDXRender::create_render_with_fourcc(DWORD t_dw_fourcc, bool t_b_reverse)
{
	HRESULT t_hr = S_FALSE;
	// Create vertex buffer
	s_vertex vertices[] = {

		XMFLOAT3(-1.0f, 1.0f, 0.5f),  XMFLOAT2(0.0f, 0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.5f),   XMFLOAT2(1.0f, 0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.5f), XMFLOAT2(0.0f, 1.0f),
		XMFLOAT3(1.0f, -1.0f, 0.5f),  XMFLOAT2(1.0f, 1.0f),

	};
	if (t_b_reverse) {
		vertices[0].m_tex = XMFLOAT2(0.0f, 1.0f);
		vertices[1].m_tex = XMFLOAT2(1.0f, 1.0f);
		vertices[2].m_tex = XMFLOAT2(0.0f, 0.0f);
		vertices[3].m_tex = XMFLOAT2(1.0f, 0.0f);
	}

	//ZeroMemory(&m_vb_bd,sizeof(m_vb_bd));
	m_vb_bd.Usage = D3D11_USAGE_DEFAULT;
	m_vb_bd.ByteWidth = sizeof(s_vertex) * 4;
	m_vb_bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vb_bd.CPUAccessFlags = 0;
	m_vb_bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA t_initial_data;
	ZeroMemory(&t_initial_data, sizeof(t_initial_data));
	t_initial_data.pSysMem = vertices;
	t_hr = m_p_device->CreateBuffer(&m_vb_bd, &t_initial_data,
					&m_p_vertex_buffer);
	if (FAILED(t_hr))
		return false;

#ifndef BGR24_CPU
	if (t_dw_fourcc == MWFOURCC_YUY2 || t_dw_fourcc == MWFOURCC_UYVY ||
	    t_dw_fourcc == MWFOURCC_BGR24) {
#else
	if (t_dw_fourcc == MWFOURCC_YUY2 || t_dw_fourcc == MWFOURCC_UYVY ||
	    t_dw_fourcc == MWFOURCC_NV12) {
#endif // !BGR24_CPU \
	//create cb_buffer
		m_d3db_rect = NULL;
		// Create the constant buffers
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(s_rect);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		t_hr = m_p_device->CreateBuffer(&bd, NULL, &m_d3db_rect);
		if (FAILED(t_hr))
			return false;
	}
	//set vertex buffer
	UINT t_stride = sizeof(s_vertex);
	UINT t_offset = 0;
	m_p_device_context->IASetVertexBuffers(0, 1, &m_p_vertex_buffer,
					       &t_stride, &t_offset);

	//set primitive topology
	m_p_device_context->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//compile the vertex shader
	s_csvs t_vs, t_ps;
	bool t_b_ret = get_vsfs(&t_vs, &t_ps, t_dw_fourcc);

	m_p_vs_blob = NULL;
	t_hr = compile_shader_from_cs(t_vs.m_p_code, t_vs.m_n_code_len, "VS",
				      "vs_4_0", &m_p_vs_blob);
	if (FAILED(t_hr)) {
		return false;
	}

	//create the vertex shader
	t_hr = m_p_device->CreateVertexShader(m_p_vs_blob->GetBufferPointer(),
					      m_p_vs_blob->GetBufferSize(),
					      NULL, &m_p_vertex_shader);
	if (FAILED(t_hr)) {
		m_p_vs_blob->Release();
		return false;
	}

	//define the input layout
	D3D11_INPUT_ELEMENT_DESC t_arr_layout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		 D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		 D3D11_INPUT_PER_VERTEX_DATA, 0}};
	UINT t_num_elements = ARRAYSIZE(t_arr_layout);

	// Create the input layout
	t_hr = m_p_device->CreateInputLayout(t_arr_layout, t_num_elements,
					     m_p_vs_blob->GetBufferPointer(),
					     m_p_vs_blob->GetBufferSize(),
					     &m_p_vertex_layout);
	m_p_vs_blob->Release();
	if (FAILED(t_hr))
		return false;

	// Set the input layout
	m_p_device_context->IASetInputLayout(m_p_vertex_layout);

	// Compile the pixel shader
	t_hr = compile_shader_from_cs(t_ps.m_p_code, t_ps.m_n_code_len, "PS",
				      "ps_4_0", &m_p_fs_blob);
	if (FAILED(t_hr)) {
		return false;
	}

	// Create the pixel shader
	t_hr = m_p_device->CreatePixelShader(m_p_fs_blob->GetBufferPointer(),
					     m_p_fs_blob->GetBufferSize(), NULL,
					     &m_p_fragment_shader);
	m_p_fs_blob->Release();
	if (FAILED(t_hr))
		return false;

	if (t_dw_fourcc == MWFOURCC_BGR24) {
		m_n_stride = FOURCC_CalcMinStride(MWFOURCC_BGRA, m_n_width, 4);
		int t_n_stride =
			FOURCC_CalcMinStride(t_dw_fourcc, m_n_width, 4);
		m_n_size = FOURCC_CalcImageSize(t_dw_fourcc, m_n_width,
						m_n_height, t_n_stride);
		int t_n_size = FOURCC_CalcImageSize(MWFOURCC_BGRA, m_n_width,
						    m_n_height, m_n_stride);
		m_n_rc_size = t_n_size;
	} else {
		m_n_stride = FOURCC_CalcMinStride(t_dw_fourcc, m_n_width, 4);
		m_n_size = FOURCC_CalcImageSize(t_dw_fourcc, m_n_width,
						m_n_height, m_n_stride);
		m_n_rc_size = m_n_size;
	}
	m_p_data = new unsigned char[m_n_rc_size];
	m_p_data2 = new unsigned char[m_n_rc_size];
	memset(m_p_data, 0, m_n_rc_size);
	memset(m_p_data2, 0, m_n_rc_size);

	// Create the render target texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_n_width;
	desc.Height = m_n_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	if (t_dw_fourcc == MWFOURCC_YUY2 || t_dw_fourcc == MWFOURCC_UYVY) {
		desc.Format =
			DXGI_FORMAT_R8G8_UNORM; // DXGI_FORMAT_R16_UNORM;// DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R8G8_UNORM;//DXGI_FORMAT_R8G8B8A8_UNORM;
	} else if (t_dw_fourcc == MWFOURCC_RGBA) {
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	} else if (t_dw_fourcc == MWFOURCC_BGRA) {
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	} else if (t_dw_fourcc == MWFOURCC_BGR24) {
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	} else if (t_dw_fourcc == MWFOURCC_NV12) {
		desc.Format = DXGI_FORMAT_R8_UNORM;
	}
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_p_texture_2d = NULL;
	t_hr = m_p_device->CreateTexture2D(&desc, NULL, &m_p_texture_2d);
	if (FAILED(t_hr))
		return false;

	// Create the shader-resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = D3DX11_DEFAULT; //1; //D3DX11_DEFAULT;

	t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d, &srDesc,
						    &m_p_shader_resource_view);
	if (FAILED(t_hr))
		return false;

	if (t_dw_fourcc == MWFOURCC_NV12) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height / 2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8G8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT; //D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET |
				  D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL,
						   &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels =
			D3DX11_DEFAULT; //1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(
			m_p_texture_2d_2, &srDesc2,
			&m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}

	ZeroMemory(&m_sampler_des, sizeof(m_sampler_des));
	m_sampler_des.Filter =
		D3D11_FILTER_ANISOTROPIC; // D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_sampler_des.AddressU =
		D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressV =
		D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressW =
		D3D11_TEXTURE_ADDRESS_WRAP; // D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.ComparisonFunc =
		D3D11_COMPARISON_ALWAYS; // D3D11_COMPARISON_NEVER;
	m_sampler_des.MinLOD = 0;
	m_sampler_des.MaxLOD = D3D11_FLOAT32_MAX;
	t_hr = m_p_device->CreateSamplerState(&m_sampler_des,
					      &m_p_sampler_state);
	if (FAILED(t_hr))
		return false;

	m_p_device_context->VSSetShader(m_p_vertex_shader, NULL, 0);
	m_p_device_context->PSSetShader(m_p_fragment_shader, NULL, 0);

	s_rect t_rect;
	t_rect.m_cx = m_n_width;
	t_rect.m_cy = m_n_height;
#ifndef BGR24_CPU
	if (t_dw_fourcc == MWFOURCC_YUY2 || t_dw_fourcc == MWFOURCC_BGR24 ||
	    t_dw_fourcc == MWFOURCC_UYVY) {
#else
	if (t_dw_fourcc == MWFOURCC_YUY2 || t_dw_fourcc == MWFOURCC_UYVY ||
	    t_dw_fourcc == MWFOURCC_NV12) {
#endif // !BGR24_CPU
		m_p_device_context->PSSetConstantBuffers(0, 1, &m_d3db_rect);
		m_p_device_context->UpdateSubresource(m_d3db_rect, 0, NULL,
						      &t_rect, 0, 0);
	}

	if (t_dw_fourcc == MWFOURCC_NV12) {
		m_p_device_context->PSSetShaderResources(
			0, 1, &m_p_shader_resource_view);
		m_p_device_context->PSSetShaderResources(
			1, 1, &m_p_shader_resource_view_2);
	} else {
		m_p_device_context->PSSetShaderResources(
			0, 1, &m_p_shader_resource_view);
	}
	m_p_device_context->PSSetSamplers(0, 1, &m_p_sampler_state);
	m_b_set = true;
	return m_b_set;
	return false;
}

void MWDXRender::destory_render_with_fourcc(DWORD t_dw_fourcc) {}

bool MWDXRender::initialize(int t_n_width, int t_n_height, DWORD dwFourcc,
			    bool t_b_reverse, HWND t_hwnd)
{
	bool t_b_ret = false;
	m_b_set = false;
	HRESULT t_hr = S_FALSE;

	m_n_width = t_n_width;
	m_n_height = t_n_height;

	m_dw_fourcc = dwFourcc;
	//define initial parameters for the swap chain
	ZeroMemory(&m_sd, sizeof(m_sd));
	m_sd.BufferCount = 2;
	m_sd.BufferDesc.Width = t_n_width;
	m_sd.BufferDesc.Height = t_n_height;
	m_sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_sd.BufferDesc.RefreshRate.Numerator = 0;
	m_sd.BufferDesc.RefreshRate.Denominator = 1;
	m_sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_sd.OutputWindow = t_hwnd;
	m_sd.SampleDesc.Count = 1;
	m_sd.SampleDesc.Quality = 0;
	m_sd.Windowed = TRUE;
	m_sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	m_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_sd.Flags = 0;

	//request a feature level that implements the features setted
	m_featurelevels = D3D_FEATURE_LEVEL_11_0; //D3D_FEATURE_LEVEL_11_0;

	//create the device
	t_hr = D3D11CreateDeviceAndSwapChain(
		NULL, /*default adapter*/
		D3D_DRIVER_TYPE_HARDWARE /*D3D_DRIVER_TYPE_REFERENCE*/, NULL,
		D3D11_CREATE_DEVICE_SINGLETHREADED, NULL, 0, D3D11_SDK_VERSION,
		&m_sd, &m_p_swap_chain, &m_p_device, &m_featurelevels,
		&m_p_device_context);

	if (FAILED(t_hr))
		return false;

	//create a target view
	m_p_render_buffer = NULL;
	t_hr = m_p_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D),
					 (LPVOID *)&m_p_render_buffer);
	if (FAILED(t_hr))
		return false;

	t_hr = m_p_device->CreateRenderTargetView(m_p_render_buffer, NULL,
						  &m_p_render_view);
	m_p_render_buffer->Release();
	if (FAILED(t_hr))
		return false;
	m_p_device_context->OMSetRenderTargets(1, &m_p_render_view, NULL);

	//setup viewport
	ZeroMemory(&m_vp, sizeof(m_vp));
	m_vp.Width = (float)t_n_width;
	m_vp.Height = (float)t_n_height;
	m_vp.MinDepth = 0.0f;
	m_vp.MaxDepth = 1.0f;
	m_vp.TopLeftX = 0.0f;
	m_vp.TopLeftY = 0.0f;
	m_p_device_context->RSSetViewports(1, &m_vp);

	//create vertex buffer
	memset(m_arr_vex, 0, sizeof(XMFLOAT3) * 3);
	m_arr_vex[0] = XMFLOAT3(0.0f, 0.5f, 0.5f);
	m_arr_vex[1] = XMFLOAT3(0.5f, -0.5f, 0.5f);
	m_arr_vex[2] = XMFLOAT3(-0.5f, -0.5f, 0.5f);

	create_render_with_fourcc(dwFourcc, t_b_reverse);

	return m_b_set;
}

void MWDXRender::cleanup_device()
{
	if (m_p_device_context != NULL) {
		m_p_device_context->Release();
		m_p_device_context = NULL;
	}

	if (m_p_vertex_buffer != NULL) {
		m_p_vertex_buffer->Release();
		m_p_vertex_buffer = NULL;
	}

	if (m_p_vertex_layout != NULL) {
		m_p_vertex_layout->Release();
		m_p_vertex_layout = NULL;
	}

	if (m_p_vertex_shader != NULL) {
		m_p_vertex_shader->Release();
		m_p_vertex_shader = NULL;
	}

	if (m_p_sampler_state != NULL) {
		m_p_sampler_state->Release();
		m_p_sampler_state = NULL;
	}

	if (m_d3db_rect != NULL) {
		m_d3db_rect->Release();
		m_d3db_rect = NULL;
	}

	if (m_p_texture_2d != NULL) {
		m_p_texture_2d->Release();
		m_p_texture_2d = NULL;
	}

	if (m_p_shader_resource_view != NULL) {
		m_p_shader_resource_view->Release();
		m_p_shader_resource_view = NULL;
	}

	if (m_p_texture_2d_2 != NULL) {
		m_p_texture_2d_2->Release();
		m_p_texture_2d_2 = NULL;
	}

	if (m_p_shader_resource_view_2 != NULL) {
		m_p_shader_resource_view_2->Release();
		m_p_shader_resource_view_2 = NULL;
	}

	if (m_p_fragment_shader != NULL) {
		m_p_fragment_shader->Release();
		m_p_fragment_shader = NULL;
	}

	if (m_p_render_view != NULL) {
		m_p_render_view->Release();
		m_p_render_view = NULL;
	}

	if (m_p_swap_chain != NULL) {
		m_p_swap_chain->Release();
		m_p_swap_chain = NULL;
	}

	if (m_p_device_context != NULL) {
		m_p_device_context->Release();
		m_p_device_context = NULL;
	}

	if (m_p_device != NULL) {
		m_p_device->Release();
		m_p_device = NULL;
	}

	if (m_p_data != NULL) {
		delete m_p_data;
		m_p_data = NULL;
	}

	if (m_p_data2 != NULL) {
		delete m_p_data2;
		m_p_data2 = NULL;
	}
}

bool MWDXRender::paint(unsigned char *data)
{
	bool t_b_ret = true;
	HRESULT t_hr = S_OK;

	//clear buffer
	if (m_b_set == false)
		return false;
	float t_clearcolor[4] = {1.0f, 1.0f, 0.0f, 1.0f};
	m_p_device_context->ClearRenderTargetView(m_p_render_view,
						  t_clearcolor);

	if (m_dw_fourcc == MWFOURCC_BGR24) {
#ifndef BGR24_CPU
		memcpy(m_p_data, data, m_n_size);
#else
		LONG lSrcStride = m_n_width * 3;
		LONG lDestStride = m_n_width * 4;
		unsigned char *t_p_src = data;
		unsigned char *t_p_des = m_p_data;
		for (DWORD y = 0; y < m_n_height; y++) {
			RGBTRIPLE *pSrcPel = (RGBTRIPLE *)t_p_src;
			DWORD *pDestPel = (DWORD *)t_p_des;

			for (DWORD x = 0; x < m_n_width; x++) {
				pDestPel[x] =
					D3DCOLOR_XRGB(pSrcPel[x].rgbtRed,
						      pSrcPel[x].rgbtGreen,
						      pSrcPel[x].rgbtBlue);
			}

			t_p_src += lSrcStride;
			t_p_des += lDestStride;
		}
#endif // !BGR24_CPU
	} else {
		memcpy(m_p_data, data, m_n_size);
	}

	if (m_dw_fourcc == MWFOURCC_NV12) {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL,
						      m_p_data, m_n_width, 0);
		m_p_device_context->UpdateSubresource(
			m_p_texture_2d_2, 0, NULL,
			m_p_data + m_n_width * m_n_height, m_n_width, 0);
	} else {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL,
						      m_p_data, m_n_stride, 0);
	}
	m_p_device_context->Draw(4, 0);
	t_hr = m_p_swap_chain->Present(0, 0);
	if (FAILED(t_hr))
		t_b_ret = false;
	return t_b_ret;
}