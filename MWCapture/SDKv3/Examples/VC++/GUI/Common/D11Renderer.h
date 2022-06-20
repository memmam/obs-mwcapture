
#pragma once

#include <d3d11.h>
//#include <d3dx11.h>

#include <vector>

#include "MWFOURCC.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)                 \
	{                               \
		if (p) {                \
			(p)->Release(); \
			(p) = NULL;     \
		}                       \
	}
#endif

class CD11Renderer {
public:
	CD11Renderer()
	{
		m_pSwapChain = NULL;
		m_pDevice = NULL;
		m_pImmediateContext = NULL;

		m_pBackBuffer = NULL;
		m_pRenderView = NULL;

		m_pDepthView = NULL;
		m_pEventQuery = NULL;
	}
	virtual ~CD11Renderer() {}

public:
	BOOL Init(int cx, int cy, DWORD dwFourcc, HWND hWnd)
	{
		HRESULT hr = S_OK;

		DXGI_SWAP_CHAIN_DESC sd = {0};
		if (dwFourcc == MWFOURCC_RGBA)
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (dwFourcc == MWFOURCC_BGRA)
			sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		else {
			return FALSE;
		}

		sd.BufferCount = 2;
		sd.BufferDesc.Width = cx;
		sd.BufferDesc.Height = cy;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;

		D3D_FEATURE_LEVEL featureLevel =
			D3D_FEATURE_LEVEL_11_0; //D3D_FEATURE_LEVEL_11_0;

		hr = D3D11CreateDeviceAndSwapChain(
			NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
			D3D11_CREATE_DEVICE_SINGLETHREADED, NULL, 0,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice,
			&featureLevel, &m_pImmediateContext);

		if (FAILED(hr))
			return FALSE;

		BOOL bRet = FALSE;
		do {
			// Create a render target view
			m_pBackBuffer = NULL;
			hr = m_pSwapChain->GetBuffer(0,
						     __uuidof(ID3D11Texture2D),
						     (LPVOID *)&m_pBackBuffer);
			if (FAILED(hr))
				break;

			hr = m_pDevice->CreateRenderTargetView(
				m_pBackBuffer, NULL, &m_pRenderView);
			if (FAILED(hr))
				break;

			// Create depth stencil texture
			ID3D11Texture2D *pDepthStencil = NULL;
			D3D11_TEXTURE2D_DESC descDepth;
			descDepth.Width = cx;
			descDepth.Height = cy;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D16_UNORM;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;
			hr = m_pDevice->CreateTexture2D(&descDepth, NULL,
							&pDepthStencil);
			if (FAILED(hr))
				break;

			// Create the depth stencil view
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			descDSV.Flags = 0;
			hr = m_pDevice->CreateDepthStencilView(
				pDepthStencil, &descDSV, &m_pDepthView);
			if (FAILED(hr))
				break;

			if (pDepthStencil)
				pDepthStencil->Release();

			m_pImmediateContext->OMSetRenderTargets(
				1, &m_pRenderView, m_pDepthView);

			// Set up the viewport
			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)cx;
			vp.Height = (FLOAT)cy;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			m_pImmediateContext->RSSetViewports(1, &vp);

			D3D11_QUERY_DESC pQueryDesc;
			pQueryDesc.Query = D3D11_QUERY_EVENT;
			pQueryDesc.MiscFlags = 0;
			hr = m_pDevice->CreateQuery(&pQueryDesc,
						    &m_pEventQuery);
			if (FAILED(hr))
				break;

			m_cx = cx;
			m_cy = cy;
			bRet = TRUE;
		} while (FALSE);

		if (!bRet)
			Exit();

		return bRet;
	}
	void Exit()
	{
		DestroyTexture();

		SAFE_RELEASE(m_pEventQuery);
		SAFE_RELEASE(m_pDepthView);

		SAFE_RELEASE(m_pRenderView);
		SAFE_RELEASE(m_pBackBuffer);

		SAFE_RELEASE(m_pImmediateContext);
		SAFE_RELEASE(m_pSwapChain);
		SAFE_RELEASE(m_pDevice);
	}

	BOOL CreateTexture(int nCount, const void *pSysMem)
	{
		m_arrTexture.assign(nCount, NULL);

		for (int i = 0; i < nCount; i++) {
			D3D11_TEXTURE2D_DESC desc_texture = {0};
			desc_texture.Width = m_cx;
			desc_texture.Height = m_cy;
			desc_texture.MipLevels = 1;
			desc_texture.ArraySize = 1;
			desc_texture.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc_texture.SampleDesc.Count = 1;
			desc_texture.SampleDesc.Quality = 0;
			desc_texture.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc_texture.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA InitialData_texture;
			InitialData_texture.SysMemPitch = desc_texture.Width;
			InitialData_texture.SysMemSlicePitch = 0;
			InitialData_texture.pSysMem = pSysMem;

			HRESULT hr = m_pDevice->CreateTexture2D(
				&desc_texture, &InitialData_texture,
				&m_arrTexture[i]);
			if (FAILED(hr))
				return FALSE;
		}

		return TRUE;
	}
	void DestroyTexture()
	{
		int nCount = (int)m_arrTexture.size();
		for (int i = 0; i < nCount; i++) {
			SAFE_RELEASE(m_arrTexture[i]);
		}
		m_arrTexture.clear();
	}

	ID3D11Device *GetDevice() { return m_pDevice; }
	ID3D11Resource *GetResource(int nIndex) { return m_arrTexture[nIndex]; }

	BOOL Render(int nIndex)
	{
		m_pImmediateContext->CopyResource(m_pBackBuffer,
						  m_arrTexture[nIndex]);
		m_pSwapChain->Present(0, 0);
		return TRUE;
	}

	BOOL Update(const void *pData, int cbStride)
	{
		m_pImmediateContext->UpdateSubresource(m_pBackBuffer, 0, NULL,
						       pData, cbStride, 0);
		HRESULT hr = m_pSwapChain->Present(0, 0);
		//It may return
		//S_OK,
		//DXGI_ERROR_DEVICE_RESET 7
		//or DXGI_ERROR_DEVICE_REMOVED(see DXGI_ERROR), 5
		//DXGI_STATUS_OCCLUDED(see DXGI_STATUS), 1
		//or D3DDDIERR_DEVICEREMOVED.
		return hr == S_OK ? TRUE : FALSE;
	}

protected:
	ID3D11Device *m_pDevice;
	ID3D11DeviceContext *m_pImmediateContext;
	IDXGISwapChain *m_pSwapChain;
	ID3D11RenderTargetView *m_pRenderView;
	ID3D11DepthStencilView *m_pDepthView;

	ID3D11Texture2D *m_pBackBuffer;

	ID3D11Query *m_pEventQuery;

	std::vector<ID3D11Texture2D *> m_arrTexture;

	int m_cx;
	int m_cy;
};
