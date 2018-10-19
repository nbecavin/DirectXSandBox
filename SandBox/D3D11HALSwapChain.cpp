#include "stdafx.h"
#include <Renderer.h>
#include <DXRenderer.h>
#include <D3D11HAL.h>
#include <WinMain.h>

void D3D11HAL::Init(int sizeX, int sizeY, sys::Renderer* owner)
{
	// Make a window rect with a client rect that is the same size as the backbuffer
	RECT rcWindow = { 0 };
	rcWindow.right = (long)(sizeX);
	rcWindow.bottom = (long)(sizeY);
	AdjustWindowRect(&rcWindow, GetWindowLong(sys::pc::hWnd, GWL_STYLE), true);

	// Resize the window.  It is important to adjust the window size 
	// after resetting the device rather than beforehand to ensure 
	// that the monitor resolution is correct and does not limit the size of the new window.
	int cx = (int)(rcWindow.right - rcWindow.left);
	int cy = (int)(rcWindow.bottom - rcWindow.top);
	SetWindowPos(sys::pc::hWnd, 0, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOMOVE);

	HRESULT hr;

	D3D_FEATURE_LEVEL returnedFeatureLevel;
	U32 Flags = 0;
#ifdef _DEBUG
		Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, Flags, NULL, 0, D3D11_SDK_VERSION, m_Device.GetAddressOf(), &returnedFeatureLevel, m_ImmediateDeviceContext.GetAddressOf());

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = sizeX;
	sd.BufferDesc.Height = sizeY;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = sys::pc::hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(&m_DxgiFactory));
	hr = m_DxgiFactory->CreateSwapChain(GetDevice(), &sd, m_SwapChain.GetAddressOf());

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return ;

	hr = GetDevice()->CreateRenderTargetView(pBackBuffer, NULL, &m_BackBuffer);
	pBackBuffer->Release();
	if (FAILED(hr))
		return ;

	// Create depth buffer
	//hr = Device->CreateDepthStencilView( pDepthBuffer, 

	/*         texturefmt = DXGI_FORMAT_R24G8_TYPELESS ;
			SRVfmt = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		   DSVfmt = DXGI_FORMAT_D24_UNORM_S8_UINT;
		*/

		// Create depth stencil texture
	ID3D11Texture2D* pDepthStencil = NULL;
	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.Width = sizeX;
	descDepth.Height = sizeY;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = GetDevice()->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
		return ;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	descDSV.Flags = 0;
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//descDepth.Format;
	if (descDepth.SampleDesc.Count > 1)
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	else
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = GetDevice()->CreateDepthStencilView(pDepthStencil, &descDSV, &m_DepthBuffer);
	if (FAILED(hr))
		return ;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;
	hr = GetDevice()->CreateShaderResourceView(pDepthStencil, &SRVDesc, &m_ZBuffer);
	if (FAILED(hr))
		return ;
}

void D3D11HAL::Shut()
{
	m_ImmediateDeviceContext->ClearState();
	m_ImmediateDeviceContext->Release();
	m_Device.Get()->Release();
	m_SwapChain.Get()->Release();
}

void D3D11HAL::SetAndClearRenderTarget()
{
	m_ImmediateDeviceContext->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);
	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	m_ImmediateDeviceContext->ClearRenderTargetView(m_BackBuffer, ClearColor);
	m_ImmediateDeviceContext->ClearDepthStencilView(m_DepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
}

void D3D11HAL::PresentFrame()
{
	m_SwapChain->Present(1, 0);
}
