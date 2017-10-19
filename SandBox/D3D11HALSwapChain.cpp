#include "stdafx.h"
#if defined(_PCDX11)
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
	//	Flags |= D3D11_CREATE_DEVICE_DEBUG;
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

}

void D3D11HAL::Shut()
{
	m_Device.Get()->Release();
	m_SwapChain.Get()->Release();
}

void D3D11HAL::PresentFrame()
{
	m_SwapChain->Present(1, 0);
}

#endif
