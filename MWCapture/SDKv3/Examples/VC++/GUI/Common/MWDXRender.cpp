#include "stdafx.h"
#include "MWDXRender.h"
#include <Windows.h>
#include <WinGDI.h>
#include <d3d9.h>

#include "MWDXRenderShader.h"

bool MWDXRender::get_vsfs(s_csvs *t_vs,s_csvs *t_ps,DWORD t_dw_fourcc){
	bool t_ret=false;

	switch (t_dw_fourcc){
	case MWFOURCC_GREY:
	case MWFOURCC_Y800:
	case MWFOURCC_Y8:
		t_vs->m_p_code=g_cs_vs_grey;
		t_vs->m_n_code_len=sizeof(g_cs_vs_grey);
		t_ps->m_p_code=g_cs_ps_grey;
		t_ps->m_n_code_len=sizeof(g_cs_ps_grey);
		t_ret=true;
		break;
	case MWFOURCC_Y16:
		t_vs->m_p_code=g_cs_vs_y16;
		t_vs->m_n_code_len=sizeof(g_cs_vs_y16);
		t_ps->m_p_code=g_cs_ps_y16;
		t_ps->m_n_code_len=sizeof(g_cs_ps_y16);
		t_ret=true;
		break;
	case MWFOURCC_RGB16:
		t_vs->m_p_code=g_cs_vs_rgb16;
		t_vs->m_n_code_len=sizeof(g_cs_vs_rgb16);
		t_ps->m_p_code=g_cs_ps_rgb16;
		t_ps->m_n_code_len=sizeof(g_cs_ps_rgb16);
		t_ret=true;
		break;
	case MWFOURCC_BGR16:
		t_vs->m_p_code=g_cs_vs_bgr16;
		t_vs->m_n_code_len=sizeof(g_cs_vs_bgr16);
		t_ps->m_p_code=g_cs_ps_bgr16;
		t_ps->m_n_code_len=sizeof(g_cs_ps_bgr16);
		t_ret=true;
		break;
	case MWFOURCC_RGBA:
		t_vs->m_p_code=g_cs_vs_rgba;
		t_vs->m_n_code_len=sizeof(g_cs_vs_rgba);
		t_ps->m_p_code=g_cs_ps_rgba;
		t_ps->m_n_code_len=sizeof(g_cs_ps_rgba);
		t_ret=true;
		break;
	case MWFOURCC_ARGB:
		t_vs->m_p_code=g_cs_vs_argb;
		t_vs->m_n_code_len=sizeof(g_cs_vs_argb);
		t_ps->m_p_code=g_cs_ps_argb;
		t_ps->m_n_code_len=sizeof(g_cs_ps_argb);
		t_ret=true;
		break;
	case MWFOURCC_BGRA:
		t_vs->m_p_code=g_cs_vs_bgra;
		t_vs->m_n_code_len=sizeof(g_cs_vs_bgra);
		t_ps->m_p_code=g_cs_ps_bgra;
		t_ps->m_n_code_len=sizeof(g_cs_ps_bgra);
		t_ret=true;
		break;
	case MWFOURCC_ABGR:
		t_vs->m_p_code=g_cs_vs_abgr;
		t_vs->m_n_code_len=sizeof(g_cs_vs_abgr);
		t_ps->m_p_code=g_cs_ps_abgr;
		t_ps->m_n_code_len=sizeof(g_cs_ps_abgr);
		t_ret=true;
		break;
	case MWFOURCC_YUY2:
	case MWFOURCC_YUYV:
		t_vs->m_p_code=g_cs_vs_yuy2;
		t_vs->m_n_code_len=sizeof(g_cs_vs_yuy2);
		t_ps->m_p_code=g_cs_ps_yuy2;
		t_ps->m_n_code_len=sizeof(g_cs_ps_yuy2);
		t_ret=true;
		break;
	case MWFOURCC_BGR24:
		t_vs->m_p_code=g_cs_vs_bgr24;
		t_vs->m_n_code_len=sizeof(g_cs_vs_bgr24);
		t_ps->m_p_code=g_cs_ps_bgr24;
		t_ps->m_n_code_len=sizeof(g_cs_ps_bgr24);
		t_ret=true;
		break;
	case MWFOURCC_RGB24:
		t_vs->m_p_code=g_cs_vs_rgb24;
		t_vs->m_n_code_len=sizeof(g_cs_vs_rgb24);
		t_ps->m_p_code=g_cs_ps_rgb24;
		t_ps->m_n_code_len=sizeof(g_cs_ps_rgb24);
		t_ret=true;
		break;
	case MWFOURCC_UYVY:
		t_vs->m_p_code=g_cs_vs_uyvy;
		t_vs->m_n_code_len=sizeof(g_cs_vs_uyvy);
		t_ps->m_p_code=g_cs_ps_uyvy;
		t_ps->m_n_code_len=sizeof(g_cs_ps_uyvy);
		t_ret=true;
		break;
	case MWFOURCC_YVYU:
		t_vs->m_p_code=g_cs_vs_yvyu;
		t_vs->m_n_code_len=sizeof(g_cs_vs_yvyu);
		t_ps->m_p_code=g_cs_ps_yvyu;
		t_ps->m_n_code_len=sizeof(g_cs_ps_yvyu);
		t_ret=true;
		break;
	case MWFOURCC_VYUY:
		t_vs->m_p_code=g_cs_vs_vyuy;
		t_vs->m_n_code_len=sizeof(g_cs_vs_vyuy);
		t_ps->m_p_code=g_cs_ps_vyuy;
		t_ps->m_n_code_len=sizeof(g_cs_ps_vyuy);
		t_ret=true;
		break;
	case MWFOURCC_NV12:
		t_vs->m_p_code = g_cs_vs_nv12;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv12);
		t_ps->m_p_code = g_cs_ps_nv12;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv12);
		t_ret=true;
		break;
	case MWFOURCC_NV21:
		t_vs->m_p_code = g_cs_vs_nv21;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv21);
		t_ps->m_p_code = g_cs_ps_nv21;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv21);
		t_ret=true;
		break;
	case MWFOURCC_NV16:
		t_vs->m_p_code = g_cs_vs_nv16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv16);
		t_ps->m_p_code = g_cs_ps_nv16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv16);
		t_ret=true;
		break;
	case MWFOURCC_NV61:
		t_vs->m_p_code = g_cs_vs_nv61;
		t_vs->m_n_code_len = sizeof(g_cs_vs_nv61);
		t_ps->m_p_code = g_cs_ps_nv61;
		t_ps->m_n_code_len = sizeof(g_cs_ps_nv61);
		t_ret=true;
		break;
	case MWFOURCC_I422:
		t_vs->m_p_code = g_cs_vs_i422;
		t_vs->m_n_code_len = sizeof(g_cs_vs_i422);
		t_ps->m_p_code = g_cs_ps_i422;
		t_ps->m_n_code_len = sizeof(g_cs_ps_i422);
		t_ret=true;
		break;
	case MWFOURCC_YV16:
		t_vs->m_p_code = g_cs_vs_yv16;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yv16);
		t_ps->m_p_code = g_cs_ps_yv16;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yv16);
		t_ret=true;
		break;
	case MWFOURCC_I420:
	case MWFOURCC_IYUV:
		t_vs->m_p_code = g_cs_vs_i420;
		t_vs->m_n_code_len = sizeof(g_cs_vs_i420);
		t_ps->m_p_code = g_cs_ps_i420;
		t_ps->m_n_code_len = sizeof(g_cs_ps_i420);
		t_ret=true;
		break;
	case MWFOURCC_YV12:
		t_vs->m_p_code = g_cs_vs_yv12;
		t_vs->m_n_code_len = sizeof(g_cs_vs_yv12);
		t_ps->m_p_code = g_cs_ps_yv12;
		t_ps->m_n_code_len = sizeof(g_cs_ps_yv12);
		t_ret=true;
		break;
	case MWFOURCC_P010:
		t_vs->m_p_code = g_cs_vs_p010;
		t_vs->m_n_code_len = sizeof(g_cs_vs_p010);
		t_ps->m_p_code = g_cs_ps_p010;
		t_ps->m_n_code_len = sizeof(g_cs_ps_p010);
		t_ret=true;
		break;
	case MWFOURCC_P210:
		t_vs->m_p_code = g_cs_vs_p210;
		t_vs->m_n_code_len = sizeof(g_cs_vs_p210);
		t_ps->m_p_code = g_cs_ps_p210;
		t_ps->m_n_code_len = sizeof(g_cs_ps_p210);
		t_ret = true;
		break;
	case MWFOURCC_IYU2:
		t_vs->m_p_code = g_cs_vs_iyu2;
		t_vs->m_n_code_len = sizeof(g_cs_vs_iyu2);
		t_ps->m_p_code = g_cs_ps_iyu2;
		t_ps->m_n_code_len = sizeof(g_cs_ps_iyu2);
		t_ret = true;
		break;
	case MWFOURCC_V308:
		t_vs->m_p_code = g_cs_vs_v308;
		t_vs->m_n_code_len = sizeof(g_cs_vs_v308);
		t_ps->m_p_code = g_cs_ps_v308;
		t_ps->m_n_code_len = sizeof(g_cs_ps_v308);
		t_ret = true;
		break;
	case MWFOURCC_AYUV:
		t_vs->m_p_code = g_cs_vs_ayuv;
		t_vs->m_n_code_len = sizeof(g_cs_vs_ayuv);
		t_ps->m_p_code = g_cs_ps_ayuv;
		t_ps->m_n_code_len = sizeof(g_cs_ps_ayuv);
		t_ret = true;
		break;
	case MWFOURCC_UYVA:
	case MWFOURCC_V408:
		t_vs->m_p_code = g_cs_vs_uyva;
		t_vs->m_n_code_len = sizeof(g_cs_vs_uyva);
		t_ps->m_p_code = g_cs_ps_uyva;
		t_ps->m_n_code_len = sizeof(g_cs_ps_uyva);
		t_ret = true;
		break;
	case MWFOURCC_VYUA:
		t_vs->m_p_code = g_cs_vs_vyua;
		t_vs->m_n_code_len = sizeof(g_cs_vs_vyua);
		t_ps->m_p_code = g_cs_ps_vyua;
		t_ps->m_n_code_len = sizeof(g_cs_ps_vyua);
		t_ret = true;
		break;
	case MWFOURCC_Y410:
		t_vs->m_p_code = g_cs_vs_y410;
		t_vs->m_n_code_len = sizeof(g_cs_vs_y410);
		t_ps->m_p_code = g_cs_ps_y410;
		t_ps->m_n_code_len = sizeof(g_cs_ps_y410);
		t_ret = true;
		break;
	case MWFOURCC_RGB10:
		t_vs->m_p_code = g_cs_vs_rgb10;
		t_vs->m_n_code_len = sizeof(g_cs_vs_rgb10);
		t_ps->m_p_code = g_cs_ps_rgb10;
		t_ps->m_n_code_len = sizeof(g_cs_ps_rgb10);
		t_ret = true;
		break;
	case MWFOURCC_BGR10:
		t_vs->m_p_code = g_cs_vs_bgr10;
		t_vs->m_n_code_len = sizeof(g_cs_vs_bgr10);
		t_ps->m_p_code = g_cs_ps_bgr10;
		t_ps->m_n_code_len = sizeof(g_cs_ps_bgr10);
		t_ret = true;
		break;
	default:
		t_vs->m_p_code=g_cs_vs_bgra;
		t_vs->m_n_code_len=sizeof(g_cs_vs_bgra);
		t_ps->m_p_code=g_cs_ps_bgra;
		t_ps->m_n_code_len=sizeof(g_cs_ps_bgra);
		t_ret=false;
		break;
	}

	return t_ret;
}

MWDXRender::MWDXRender()
{
	for(int i=0;i<NUM_DATA;i++)
		m_array_data[i]=NULL;

	m_p_vertex_buffer=NULL;
	m_p_vertex_layout=NULL;
	m_p_vertex_shader=NULL;
	m_p_sampler_state=NULL;
	m_d3db_rect=NULL;
	m_p_texture_2d=NULL;
	m_p_shader_resource_view=NULL;
	m_p_texture_2d_2=NULL;
	m_p_shader_resource_view_2=NULL;
	m_p_texture_2d_3=NULL;
	m_p_shader_resource_view_3=NULL;
	m_p_fragment_shader=NULL;
	m_p_render_view=NULL;
	m_p_swap_chain=NULL;
	m_p_device_context=NULL;
	m_p_device=NULL;
}

MWDXRender::~MWDXRender()
{

}


HRESULT MWDXRender::compile_shader_from_file(WCHAR *t_sz_filepath,LPCSTR t_sz_entrypoint,LPCSTR t_sz_shader_model,ID3DBlob** t_pp_blob_out)
{
	HRESULT t_hr=S_FALSE;

	DWORD t_dw_shaderflags=D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr=D3DX11CompileFromFile(t_sz_filepath,NULL,NULL,t_sz_entrypoint,
								t_sz_shader_model,t_dw_shaderflags,0,
								NULL,t_pp_blob_out,&t_p_errorblod,NULL);
	if(FAILED(t_hr)){
		if(t_p_errorblod!=NULL){
			//output the err
			//release
			//TRACE("%s\n",t_p_errorblod->GetBufferPointer());
			t_p_errorblod->Release();
			t_p_errorblod=NULL;
		}
	}
	if(t_p_errorblod){
		t_p_errorblod->Release();
		t_p_errorblod=NULL;
	}
	return t_hr;
}

HRESULT MWDXRender::compile_shader_from_cs(unsigned char*t_p_data, int t_n_data_len, LPCSTR t_sz_entrypoint, LPCSTR t_sz_shader_model, ID3DBlob** t_pp_blob_out) {
	HRESULT t_hr = S_FALSE;

	DWORD t_dw_shaderflags = D3DCOMPILE_ENABLE_STRICTNESS;

	ID3DBlob *t_p_errorblod;
	t_hr = D3DCompile(t_p_data,t_n_data_len,NULL, NULL,NULL,t_sz_entrypoint,
		t_sz_shader_model, t_dw_shaderflags, 0,
		t_pp_blob_out, &t_p_errorblod);
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

bool MWDXRender::create_render_with_fourcc(DWORD t_dw_fourcc,bool t_b_reverse, mw_color_space_e t_color_format, mw_color_space_level_e input_range, mw_color_space_level_e output_range)
{
	HRESULT t_hr=S_FALSE;
	// Create vertex buffer
	s_vertex vertices[] =
	{
		
		XMFLOAT3(-1.0f, 1.0f, 0.5f),XMFLOAT2(0.0f,0.0f),
		XMFLOAT3(1.0f, 1.0f, 0.5f),XMFLOAT2(1.0f,0.0f),
		XMFLOAT3(-1.0f, -1.0f, 0.5f),XMFLOAT2(0.0f,1.0f),
		XMFLOAT3(1.0f, -1.0f, 0.5f),XMFLOAT2(1.0f,1.0f),

	};
	if(t_b_reverse){
		vertices[0].m_tex=XMFLOAT2(0.0f,1.0f);
		vertices[1].m_tex=XMFLOAT2(1.0f,1.0f);
		vertices[2].m_tex=XMFLOAT2(0.0f,0.0f);
		vertices[3].m_tex=XMFLOAT2(1.0f,0.0f);
	}


	//ZeroMemory(&m_vb_bd,sizeof(m_vb_bd));
	m_vb_bd.Usage=D3D11_USAGE_DEFAULT;
	m_vb_bd.ByteWidth=sizeof(s_vertex)*4;
	m_vb_bd.BindFlags=D3D11_BIND_VERTEX_BUFFER;
	m_vb_bd.CPUAccessFlags=0;
	m_vb_bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA t_initial_data;
	ZeroMemory(&t_initial_data,sizeof(t_initial_data));
	t_initial_data.pSysMem=vertices;
	t_hr=m_p_device->CreateBuffer(&m_vb_bd,&t_initial_data,&m_p_vertex_buffer);
	if(FAILED(t_hr))
		return false;

	if (t_dw_fourcc == MWFOURCC_GREY|| \
		t_dw_fourcc == MWFOURCC_Y800|| \
		t_dw_fourcc == MWFOURCC_Y8|| \
		t_dw_fourcc == MWFOURCC_Y16|| \
		t_dw_fourcc == MWFOURCC_RGB16|| \
		t_dw_fourcc == MWFOURCC_BGR16|| \
		t_dw_fourcc == MWFOURCC_YUY2|| \
		t_dw_fourcc == MWFOURCC_YUYV|| \
		t_dw_fourcc == MWFOURCC_UYVY|| \
		t_dw_fourcc == MWFOURCC_YVYU|| \
		t_dw_fourcc == MWFOURCC_VYUY|| \
		t_dw_fourcc == MWFOURCC_BGR24|| \
		t_dw_fourcc ==MWFOURCC_RGB24|| \
		t_dw_fourcc == MWFOURCC_NV12|| \
		t_dw_fourcc == MWFOURCC_NV21|| \
		t_dw_fourcc == MWFOURCC_NV16|| \
		t_dw_fourcc == MWFOURCC_NV61|| \
		t_dw_fourcc == MWFOURCC_I422|| \
		t_dw_fourcc == MWFOURCC_YV16|| \
		t_dw_fourcc == MWFOURCC_I420|| \
		t_dw_fourcc == MWFOURCC_IYUV|| \
		t_dw_fourcc == MWFOURCC_YV12|| \
		t_dw_fourcc == MWFOURCC_P010|| \
		t_dw_fourcc == MWFOURCC_P210|| \
		t_dw_fourcc == MWFOURCC_IYU2|| \
		t_dw_fourcc == MWFOURCC_V308|| \
		t_dw_fourcc == MWFOURCC_AYUV|| \
		t_dw_fourcc == MWFOURCC_UYVA|| \
		t_dw_fourcc == MWFOURCC_V408|| \
		t_dw_fourcc == MWFOURCC_VYUA|| \
		t_dw_fourcc == MWFOURCC_Y410) {
		//create cb_buffer
		m_d3db_rect = NULL;
		// Create the constant buffers
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth =  sizeof(s_rect);
		bd.BindFlags =  D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		t_hr = m_p_device->CreateBuffer(&bd, NULL, &m_d3db_rect);
		if (FAILED(t_hr))
			return false;
	}
	//set vertex buffer
	UINT t_stride=sizeof(s_vertex);
	UINT t_offset=0;
	m_p_device_context->IASetVertexBuffers(0,1,&m_p_vertex_buffer,&t_stride,&t_offset);

	//set primitive topology
	m_p_device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//compile the vertex shader
	s_csvs t_vs,t_ps;
	bool t_b_ret=get_vsfs(&t_vs,&t_ps,t_dw_fourcc);

	m_p_vs_blob=NULL;
	t_hr = compile_shader_from_cs(t_vs.m_p_code, t_vs.m_n_code_len, "VS", "vs_4_0", &m_p_vs_blob);
	if(FAILED(t_hr)){
		return false;
	}

	//create the vertex shader
	t_hr=m_p_device->CreateVertexShader(m_p_vs_blob->GetBufferPointer(),m_p_vs_blob->GetBufferSize(),NULL,&m_p_vertex_shader);
	if(FAILED(t_hr)){
		m_p_vs_blob->Release();
		return false;
	}

	//define the input layout
	D3D11_INPUT_ELEMENT_DESC t_arr_layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT t_num_elements = ARRAYSIZE( t_arr_layout );

	// Create the input layout
	t_hr = m_p_device->CreateInputLayout( t_arr_layout, t_num_elements, m_p_vs_blob->GetBufferPointer(),
		m_p_vs_blob->GetBufferSize(), &m_p_vertex_layout);
	m_p_vs_blob->Release();
	if( FAILED( t_hr ) )
		return false;

	// Set the input layout
	m_p_device_context->IASetInputLayout(m_p_vertex_layout);

	// Compile the pixel shader
	t_hr = compile_shader_from_cs(t_ps.m_p_code, t_ps.m_n_code_len, "PS", "ps_4_0", &m_p_fs_blob);
	if( FAILED( t_hr ) )
	{
		return false;
	}

	// Create the pixel shader
	t_hr = m_p_device->CreatePixelShader( m_p_fs_blob->GetBufferPointer(), m_p_fs_blob->GetBufferSize(), NULL, &m_p_fragment_shader );
	m_p_fs_blob->Release();
	if( FAILED( t_hr ) )
		return false;

	if(t_dw_fourcc==MWFOURCC_BGR24|| \
	   t_dw_fourcc==MWFOURCC_RGB24|| \
	   t_dw_fourcc==MWFOURCC_IYU2|| \
	   t_dw_fourcc==MWFOURCC_V308){
		m_n_stride=FOURCC_CalcMinStride(MWFOURCC_BGRA,m_n_width,4);
		int t_n_stride=FOURCC_CalcMinStride(t_dw_fourcc,m_n_width,4);
		m_n_size=FOURCC_CalcImageSize(t_dw_fourcc,m_n_width,m_n_height,t_n_stride);
		int t_n_size=FOURCC_CalcImageSize(MWFOURCC_BGRA,m_n_width,m_n_height,m_n_stride);
		m_n_rc_size =t_n_size;
	}
	else{
		m_n_stride=FOURCC_CalcMinStride(t_dw_fourcc,m_n_width,4);
		m_n_size=FOURCC_CalcImageSize(t_dw_fourcc,m_n_width,m_n_height,m_n_stride);
		m_n_rc_size =m_n_size;
	}

	for(int i=0;i<NUM_DATA;i++){
		m_array_data[i]= new unsigned char[m_n_rc_size];
		if(m_array_data[i]!=NULL){
			memset(m_array_data[i],0,m_n_rc_size);
			m_queue_data.push(m_array_data[i]);
		}
	}

	// Create the render target texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_n_width;
	desc.Height = m_n_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;

	switch (t_dw_fourcc){
	case MWFOURCC_YUY2:
	case MWFOURCC_YUYV:
	case MWFOURCC_UYVY:
	case MWFOURCC_YVYU:
	case MWFOURCC_VYUY:
		desc.Format = DXGI_FORMAT_R8G8_UNORM;
		break;
	case MWFOURCC_RGBA:
	case MWFOURCC_BGRA:
	case MWFOURCC_ABGR:
	case MWFOURCC_ARGB:
	case MWFOURCC_BGR24:
	case MWFOURCC_RGB24:
	case MWFOURCC_IYU2:
	case MWFOURCC_V308:
	case MWFOURCC_AYUV:
	case MWFOURCC_UYVA:
	case MWFOURCC_V408:
	case MWFOURCC_VYUA:
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case MWFOURCC_Y410:
	case MWFOURCC_RGB10:
	case MWFOURCC_BGR10:
		desc.Format=DXGI_FORMAT_R10G10B10A2_UNORM;
		break;
	case MWFOURCC_GREY:
	case MWFOURCC_Y800:
	case MWFOURCC_Y8:
	case MWFOURCC_NV12:
	case MWFOURCC_NV21:
	case MWFOURCC_NV16:
	case MWFOURCC_NV61:
	case MWFOURCC_I422:
	case MWFOURCC_YV16:
	case MWFOURCC_I420:
	case MWFOURCC_IYUV:
	case MWFOURCC_YV12:
		desc.Format = DXGI_FORMAT_R8_UNORM;
		break;
	case MWFOURCC_Y16:
		desc.Format=DXGI_FORMAT_R16_UNORM;
		break;
	case MWFOURCC_RGB16:
	case MWFOURCC_BGR16:
		desc.Format = DXGI_FORMAT_B5G6R5_UNORM;
		break;
	case MWFOURCC_P010:
	case MWFOURCC_P210:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;
	default:
		desc.Format = DXGI_FORMAT_B5G6R5_UNORM;
		break;
	}


	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	m_p_texture_2d = NULL;
	t_hr=m_p_device->CreateTexture2D(&desc, NULL, &m_p_texture_2d);
	if (FAILED(t_hr))
		return false;

	// Create the shader-resource view
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = desc.Format;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

	t_hr=m_p_device->CreateShaderResourceView(m_p_texture_2d, &srDesc, &m_p_shader_resource_view);
	if (FAILED(t_hr))
		return false;

	if (t_dw_fourcc == MWFOURCC_NV12||
		t_dw_fourcc == MWFOURCC_NV21) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width/2;
		desc2.Height = m_n_height/2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;
		
		desc2.Format = DXGI_FORMAT_R8G8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}else if(
		t_dw_fourcc == MWFOURCC_NV16||
		t_dw_fourcc == MWFOURCC_NV61){
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width/2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8G8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}
	else if(t_dw_fourcc == MWFOURCC_I422|| \
			t_dw_fourcc == MWFOURCC_YV16){
		// Create the render target texture 2
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width;
		desc2.Height = m_n_height/2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;

		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc3;
		ZeroMemory(&desc3, sizeof(desc3));
		desc3.Width = m_n_width;
		desc3.Height = m_n_height/2;
		desc3.MipLevels = 1;
		desc3.ArraySize = 1;

		desc3.Format = DXGI_FORMAT_R8_UNORM;

		desc3.SampleDesc.Count = 1;
		desc3.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc3.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_3 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc3, NULL, &m_p_texture_2d_3);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc3;
		srDesc3.Format = desc3.Format;
		srDesc3.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc3.Texture2D.MostDetailedMip = 0;
		srDesc3.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_3, &srDesc3, &m_p_shader_resource_view_3);
		if (FAILED(t_hr))
			return false;
	}
	else if(t_dw_fourcc == MWFOURCC_I420|| \
			t_dw_fourcc == MWFOURCC_IYUV|| \
			t_dw_fourcc == MWFOURCC_YV12){
		// Create the render target texture 2
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width;
		desc2.Height = m_n_height/4;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R8_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;

		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc3;
		ZeroMemory(&desc3, sizeof(desc3));
		desc3.Width = m_n_width;
		desc3.Height = m_n_height/4;
		desc3.MipLevels = 1;
		desc3.ArraySize = 1;

		desc3.Format = DXGI_FORMAT_R8_UNORM;

		desc3.SampleDesc.Count = 1;
		desc3.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc3.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_3 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc3, NULL, &m_p_texture_2d_3);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc3;
		srDesc3.Format = desc3.Format;
		srDesc3.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc3.Texture2D.MostDetailedMip = 0;
		srDesc3.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_3, &srDesc3, &m_p_shader_resource_view_3);
		if (FAILED(t_hr))
			return false;
	}
	else if(t_dw_fourcc == MWFOURCC_P010){
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height / 2;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R16G16_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}
	else if (t_dw_fourcc == MWFOURCC_P210) {
		// Create the render target texture
		D3D11_TEXTURE2D_DESC desc2;
		ZeroMemory(&desc2, sizeof(desc2));
		desc2.Width = m_n_width / 2;
		desc2.Height = m_n_height;
		desc2.MipLevels = 1;
		desc2.ArraySize = 1;

		desc2.Format = DXGI_FORMAT_R16G16_UNORM;

		desc2.SampleDesc.Count = 1;
		desc2.Usage = D3D11_USAGE_DEFAULT;//D3D11_USAGE_DEFAULT;
		desc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		m_p_texture_2d_2 = NULL;
		t_hr = m_p_device->CreateTexture2D(&desc2, NULL, &m_p_texture_2d_2);
		if (FAILED(t_hr))
			return false;

		// Create the shader-resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srDesc2;
		srDesc2.Format = desc2.Format;
		srDesc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srDesc2.Texture2D.MostDetailedMip = 0;
		srDesc2.Texture2D.MipLevels = D3DX11_DEFAULT;//1; //D3DX11_DEFAULT;

		t_hr = m_p_device->CreateShaderResourceView(m_p_texture_2d_2, &srDesc2, &m_p_shader_resource_view_2);
		if (FAILED(t_hr))
			return false;
	}

	ZeroMemory(&m_sampler_des, sizeof(m_sampler_des));
	m_sampler_des.Filter = D3D11_FILTER_ANISOTROPIC;// D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_sampler_des.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; //D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;// D3D11_TEXTURE_ADDRESS_CLAMP;// D3D11_TEXTURE_ADDRESS_WRAP;
	m_sampler_des.ComparisonFunc = D3D11_COMPARISON_ALWAYS;// D3D11_COMPARISON_NEVER;
	m_sampler_des.MinLOD =  0;
	m_sampler_des.MaxLOD =  D3D11_FLOAT32_MAX;

	if (t_dw_fourcc == MWFOURCC_BGR24|| \
		t_dw_fourcc==MWFOURCC_RGB24|| \
		t_dw_fourcc==MWFOURCC_IYU2|| \
		t_dw_fourcc==MWFOURCC_V308)
	{
		m_sampler_des.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}

	t_hr = m_p_device->CreateSamplerState(&m_sampler_des, &m_p_sampler_state);
	if (FAILED(t_hr))
		return false;


	m_p_device_context->VSSetShader(m_p_vertex_shader, NULL, 0);
	m_p_device_context->PSSetShader(m_p_fragment_shader, NULL, 0);

	s_rect t_rect;
	t_rect.m_cx = m_n_width;
	t_rect.m_cy = m_n_height;
///////////////////////////////////////////////////////////////
	float csp_coeff[3][3] = { 0.0 };
	float csp_const[3] = { 0.0 };
	float lr = 0.0;
	float lg = 0.0;
	float lb = 0.0;

	m_csp = t_color_format;

	switch (m_csp)
	{
	case MW_CSP_BT_601:
		lr = 0.299, lg = 0.587, lb = 0.114;
		break;
	case MW_CSP_BT_709:
		lr = 0.2126, lg = 0.7152, lb = 0.0722;
		break;
	case MW_CSP_BT_2020:
		lr = 0.2627, lg = 0.6780, lb = 0.0593;
		break;
	}

	csp_coeff[0][0] = 1;
	csp_coeff[0][1] = 0;
	csp_coeff[0][2] = 2 * (1 - lr);
	csp_coeff[1][0] = 1;
	csp_coeff[1][1] = -2 * (1 - lb) * lb / lg;
	csp_coeff[1][2] = -2 * (1 - lr) * lr / lg;
	csp_coeff[2][0] = 1;
	csp_coeff[2][1] = 2 * (1 - lb);
	csp_coeff[2][2] = 0;

	yuvlevels yuvlev = { 0.0 };
	rgblevels rgblev = { 0.0 };
	rgblevels t_rgblev_tv = { 16.0 / 255.0, 235.0 / 255.0 };
	rgblevels t_rgblev_pc = { 0.0, 255.0 / 255.0 };
	yuvlevels t_lev_tv = { 16.0 / 255.0, 235.0 / 255.0, 240.0 / 255.0, 128.0 / 255.0 };
	yuvlevels t_lev_pc = { 0.0 , 255.0 / 255.0, 255.0 / 255.0, 128.0 / 255.0 };

	m_csp_lvl_in = input_range;
	m_csp_lvl_out = output_range;

	switch (m_csp_lvl_in)
	{
	case MW_CSP_LEVELS_AUTO:
		yuvlev = t_lev_pc;
		break;
	case MW_CSP_LEVELS_TV:
		yuvlev = t_lev_tv;
		break;
	case MW_CSP_LEVELS_PC:
		yuvlev = t_lev_pc;
		break;
	}
	switch (m_csp_lvl_out)
	{
	case MW_CSP_LEVELS_AUTO:
		yuvlev = t_lev_pc;
		break;
	case MW_CSP_LEVELS_TV:
		rgblev = t_rgblev_tv;
		break;
	case MW_CSP_LEVELS_PC: 
		rgblev = t_rgblev_pc;
		break;
	}

	double ymul = (rgblev.max - rgblev.min) / (yuvlev.ymax - yuvlev.ymin);
	double cmul = (rgblev.max - rgblev.min) / (yuvlev.cmax - yuvlev.cmid) / 2.0;

	for (int i = 0; i < 3; i++) {
		csp_coeff[i][0] *= ymul;
		csp_coeff[i][1] *= cmul;
		csp_coeff[i][2] *= cmul;

		csp_const[i] = rgblev.min - csp_coeff[i][0] * yuvlev.ymin
			- (csp_coeff[i][1] + csp_coeff[i][2]) * yuvlev.cmid;
	}
	t_rect.m_csp_coeff_0_0 = csp_coeff[0][0];
	t_rect.m_csp_coeff_0_1 = csp_coeff[0][1];
	t_rect.m_csp_coeff_0_2 = csp_coeff[0][2];
	t_rect.m_csp_coeff_1_0 = csp_coeff[1][0];
	t_rect.m_csp_coeff_1_1 = csp_coeff[1][1];
	t_rect.m_csp_coeff_1_2 = csp_coeff[1][2];
	t_rect.m_csp_coeff_2_0 = csp_coeff[2][0];
	t_rect.m_csp_coeff_2_1 = csp_coeff[2][1];
	t_rect.m_csp_coeff_2_2 = csp_coeff[2][2];

	t_rect.m_csp_const_0 = csp_const[0];
	t_rect.m_csp_const_1 = csp_const[1];
	t_rect.m_csp_const_2 = csp_const[2];


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (t_dw_fourcc == MWFOURCC_GREY|| \
		t_dw_fourcc == MWFOURCC_Y800|| \
		t_dw_fourcc == MWFOURCC_Y8|| \
		t_dw_fourcc == MWFOURCC_Y16|| \
		t_dw_fourcc == MWFOURCC_RGB16|| \
		t_dw_fourcc == MWFOURCC_BGR16|| \
		t_dw_fourcc == MWFOURCC_YUY2|| \
		t_dw_fourcc == MWFOURCC_YUYV|| \
		t_dw_fourcc == MWFOURCC_BGR24|| \
		t_dw_fourcc == MWFOURCC_RGB24|| \
		t_dw_fourcc == MWFOURCC_UYVY || \
		t_dw_fourcc == MWFOURCC_YVYU|| \
		t_dw_fourcc == MWFOURCC_VYUY|| \
		t_dw_fourcc == MWFOURCC_NV12|| \
		t_dw_fourcc == MWFOURCC_NV21|| \
		t_dw_fourcc == MWFOURCC_NV16|| \
		t_dw_fourcc == MWFOURCC_NV61|| \
		t_dw_fourcc == MWFOURCC_I422|| \
		t_dw_fourcc == MWFOURCC_YV16|| \
		t_dw_fourcc == MWFOURCC_I420|| \
		t_dw_fourcc == MWFOURCC_IYUV|| \
		t_dw_fourcc == MWFOURCC_YV12|| \
		t_dw_fourcc == MWFOURCC_P010|| \
		t_dw_fourcc == MWFOURCC_P210|| \
		t_dw_fourcc == MWFOURCC_IYU2|| \
		t_dw_fourcc == MWFOURCC_V308|| \
		t_dw_fourcc == MWFOURCC_AYUV|| \
		t_dw_fourcc == MWFOURCC_UYVA|| \
		t_dw_fourcc == MWFOURCC_V408|| \
		t_dw_fourcc == MWFOURCC_VYUA|| \
		t_dw_fourcc == MWFOURCC_Y410) {
		m_p_device_context->PSSetConstantBuffers(0, 1, &m_d3db_rect);
		m_p_device_context->UpdateSubresource(m_d3db_rect, 0, NULL, &t_rect, 0, 0);
	}

	
	if (t_dw_fourcc == MWFOURCC_NV12|| \
		t_dw_fourcc == MWFOURCC_NV21|| \
		t_dw_fourcc == MWFOURCC_NV16|| \
		t_dw_fourcc == MWFOURCC_NV61|| \
		t_dw_fourcc == MWFOURCC_P010|| \
		t_dw_fourcc == MWFOURCC_P210) {
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
		m_p_device_context->PSSetShaderResources(1, 1, &m_p_shader_resource_view_2);
	}
	else if(t_dw_fourcc == MWFOURCC_I422|| \
			t_dw_fourcc == MWFOURCC_YV16|| \
			t_dw_fourcc == MWFOURCC_I420|| \
			t_dw_fourcc == MWFOURCC_IYUV|| \
			t_dw_fourcc == MWFOURCC_YV12){
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
		m_p_device_context->PSSetShaderResources(1, 1, &m_p_shader_resource_view_2);
		m_p_device_context->PSSetShaderResources(2, 1, &m_p_shader_resource_view_3);
	}
	else {
		m_p_device_context->PSSetShaderResources(0, 1, &m_p_shader_resource_view);
	}
	m_p_device_context->PSSetSamplers(0, 1, &m_p_sampler_state);
	m_b_set = true;
	return m_b_set;
return false;
}

void MWDXRender::destory_render_with_fourcc(DWORD t_dw_fourcc)
{

}

bool MWDXRender::initialize(int t_n_width,int t_n_height,DWORD dwFourcc,bool t_b_reverse,HWND t_hwnd, 
	                        mw_color_space_e t_color_format, mw_color_space_level_e t_input_range, mw_color_space_level_e t_output_range)
{
	bool t_b_ret=false;
	m_b_set = false;
	HRESULT t_hr=S_FALSE;

	m_n_width=t_n_width;
	m_n_height=t_n_height;

	m_dw_fourcc = dwFourcc;
	//define initial parameters for the swap chain
	ZeroMemory(&m_sd,sizeof(m_sd));
	m_sd.BufferCount=2;
	m_sd.BufferDesc.Width = t_n_width;
	m_sd.BufferDesc.Height = t_n_height;
	m_sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_sd.BufferDesc.RefreshRate.Numerator=0;
	m_sd.BufferDesc.RefreshRate.Denominator=1;
	m_sd.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_sd.OutputWindow=t_hwnd;
	m_sd.SampleDesc.Count=1;
	m_sd.SampleDesc.Quality=0;
	m_sd.Windowed=TRUE;
	m_sd.BufferDesc.ScanlineOrdering=DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	m_sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_sd.Flags = 0;

	//request a feature level that implements the features setted
	m_featurelevels=D3D_FEATURE_LEVEL_11_0;//D3D_FEATURE_LEVEL_11_0;

	//create the device
	t_hr=D3D11CreateDeviceAndSwapChain(NULL, /*default adapter*/
									   D3D_DRIVER_TYPE_HARDWARE/*D3D_DRIVER_TYPE_REFERENCE*/,
									   NULL,
									   D3D11_CREATE_DEVICE_SINGLETHREADED,
									   NULL,
									   0,
									   D3D11_SDK_VERSION,
									   &m_sd,
									   &m_p_swap_chain,
									   &m_p_device,
									   &m_featurelevels,
									   &m_p_device_context);


	if(FAILED(t_hr))
		return false;

	//create a target view
	m_p_render_buffer=NULL;
	t_hr=m_p_swap_chain->GetBuffer(0,__uuidof(ID3D11Texture2D),(LPVOID*)&m_p_render_buffer);
	if(FAILED(t_hr))
		return false;

	t_hr=m_p_device->CreateRenderTargetView(m_p_render_buffer,NULL,&m_p_render_view);
	m_p_render_buffer->Release();
	if(FAILED(t_hr))
		return false;
	m_p_device_context->OMSetRenderTargets(1,&m_p_render_view,NULL);

	//setup viewport
	ZeroMemory(&m_vp,sizeof(m_vp));
	m_vp.Width = (float)t_n_width;
	m_vp.Height = (float)t_n_height;
	m_vp.MinDepth=0.0f;
	m_vp.MaxDepth=1.0f;
	m_vp.TopLeftX=0.0f;
	m_vp.TopLeftY=0.0f;
	m_p_device_context->RSSetViewports(1,&m_vp);

	//create vertex buffer
	memset(m_arr_vex,0,sizeof(XMFLOAT3)*3);
	m_arr_vex[0]=XMFLOAT3(0.0f,0.5f,0.5f);
	m_arr_vex[1]=XMFLOAT3(0.5f,-0.5f,0.5f);
	m_arr_vex[2]=XMFLOAT3(-0.5f,-0.5f,0.5f);

	create_render_with_fourcc(dwFourcc,t_b_reverse, t_color_format, t_input_range, t_output_range);

	return m_b_set;
}

void MWDXRender::cleanup_device()
{
	if(m_p_device_context!=NULL){
		m_p_device_context->Release();
		m_p_device_context=NULL;
	}

	if(m_p_vertex_buffer!=NULL){
		m_p_vertex_buffer->Release();
		m_p_vertex_buffer=NULL;
	}

	if(m_p_vertex_layout!=NULL){
		m_p_vertex_layout->Release();
		m_p_vertex_layout=NULL;
	}

	if(m_p_vertex_shader!=NULL){
		m_p_vertex_shader->Release();
		m_p_vertex_shader=NULL;
	}

	if(m_p_sampler_state!=NULL){
		m_p_sampler_state->Release();
		m_p_sampler_state=NULL;
	}

	if(m_d3db_rect!=NULL){
		m_d3db_rect->Release();
		m_d3db_rect=NULL;
	}

	if(m_p_texture_2d!=NULL){
		m_p_texture_2d->Release();
		m_p_texture_2d=NULL;
	}

	if(m_p_shader_resource_view!=NULL){
		m_p_shader_resource_view->Release();
		m_p_shader_resource_view=NULL;
	}

	if(m_p_texture_2d_2!=NULL){
		m_p_texture_2d_2->Release();
		m_p_texture_2d_2=NULL;
	}

	if(m_p_shader_resource_view_2!=NULL){
		m_p_shader_resource_view_2->Release();
		m_p_shader_resource_view_2=NULL;
	}

	if(m_p_texture_2d_3!=NULL){
		m_p_texture_2d_3->Release();
		m_p_texture_2d_3=NULL;
	}

	if(m_p_shader_resource_view_3!=NULL){
		m_p_shader_resource_view_3->Release();
		m_p_shader_resource_view_3=NULL;
	}

	if(m_p_fragment_shader!=NULL){
		m_p_fragment_shader->Release();
		m_p_fragment_shader=NULL;
	}

	if(m_p_render_view!=NULL){
		m_p_render_view->Release();
		m_p_render_view=NULL;
	}

	if(m_p_swap_chain!=NULL){
		m_p_swap_chain->Release();
		m_p_swap_chain=NULL;
	}

	if(m_p_device_context!=NULL){
		m_p_device_context->Release();
		m_p_device_context=NULL;
	}

	if(m_p_device!=NULL){
		m_p_device->Release();
		m_p_device=NULL;
	}

	queue<unsigned char*> t_queue_empty;
	t_queue_empty.swap(m_queue_data);
	for(int i=0;i<NUM_DATA;i++){
		if(m_array_data[i]!=NULL){
			delete m_array_data[i];
			m_array_data[i]=NULL;
		}
	}
}

bool MWDXRender::paint(unsigned char*data)
{
	bool t_b_ret=true;
	HRESULT t_hr=S_OK;

	//clear buffer
	if (m_b_set == false)
		return false;
	float t_clearcolor[4]={1.0f,1.0f,0.0f,1.0f};
	m_p_device_context->ClearRenderTargetView(m_p_render_view,t_clearcolor);

	/*unsigned char* t_p_data = NULL;
	if(m_queue_data.size()!=NUM_DATA)
	return false;

	t_p_data = m_queue_data.front();
	m_queue_data.pop();
	m_queue_data.push(t_p_data);
	memcpy(t_p_data,data,m_n_size);*/
	unsigned char* t_p_data = NULL;
	t_p_data = data;
	if(t_p_data == NULL)
		return false;

	if (m_dw_fourcc == MWFOURCC_NV12|| \
		m_dw_fourcc == MWFOURCC_NV21|| \
		m_dw_fourcc == MWFOURCC_NV16|| \
		m_dw_fourcc == MWFOURCC_NV61) {
			m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, 0);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_2, 0, NULL, t_p_data+m_n_width*m_n_height, m_n_width, 0);
	}
	else if(m_dw_fourcc == MWFOURCC_I422|| \
		m_dw_fourcc == MWFOURCC_YV16){
			m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, m_n_height);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_2, 0, NULL, t_p_data+m_n_width*m_n_height, m_n_width, m_n_height/2);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_3, 0, NULL, t_p_data+m_n_width*m_n_height+m_n_width*m_n_height/2, m_n_width, 0);
	}
	else if(m_dw_fourcc == MWFOURCC_I420|| \
		m_dw_fourcc == MWFOURCC_IYUV|| \
		m_dw_fourcc == MWFOURCC_YV12){
			m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width, m_n_height);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_2, 0, NULL, t_p_data+m_n_width*m_n_height, m_n_width, m_n_height/4);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_3, 0, NULL, t_p_data+m_n_width*m_n_height+m_n_width*m_n_height/4, m_n_width, 0);
	}
	else if(m_dw_fourcc == MWFOURCC_P010|| \
		m_dw_fourcc == MWFOURCC_P210){
			m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_width*2, m_n_width * 2*m_n_height);
			m_p_device_context->UpdateSubresource(m_p_texture_2d_2, 0, NULL, t_p_data+m_n_width*m_n_height*2, m_n_width*2, 0);
	}
	else {
		m_p_device_context->UpdateSubresource(m_p_texture_2d, 0, NULL, t_p_data, m_n_stride, 0);
	}

	m_p_device_context->Draw(4,0);
	t_hr=m_p_swap_chain->Present(1,0);
	if(FAILED(t_hr)) t_b_ret=false;
	return t_b_ret;
}