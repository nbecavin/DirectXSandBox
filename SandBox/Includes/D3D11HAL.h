#pragma once

using namespace Microsoft::WRL;

class D3D11HAL
{
public:
	void Init(int sizeX, int sizeY, sys::Renderer* owner);
	void Shut();

	void PresentFrame();

	ID3D11Device* GetDevice() { return m_Device.Get(); }

	// --- to remove
	ID3D11DeviceContext* GetImmediateDeviceContext() { return m_ImmediateDeviceContext.Get(); }
	IDXGISwapChain* GetSwapChain() { return m_SwapChain.Get(); }
	// --- to remove

private:
	ComPtr<IDXGIFactory1>				m_DxgiFactory;
	ComPtr<IDXGISwapChain>				m_SwapChain;
	ComPtr<ID3D11Device>				m_Device;
	ComPtr<ID3D11DeviceContext>			m_ImmediateDeviceContext;
};
