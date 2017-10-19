#pragma once

using namespace Microsoft::WRL;

class D3D12HAL
{
public:
	void Init(int sizeX,int sizeY, sys::Renderer* owner);
	void Shut();

	ID3D12Device* GetDevice() { return m_Device.Get(); }

	// --- to remove
	ID3D11DeviceContext* GetImmediateDeviceContext() { return (ID3D11DeviceContext*)nullptr; }
	IDXGISwapChain* GetSwapChain() { return m_SwapChain.Get(); }
	// --- to remove

private:
	ComPtr<IDXGIFactory4>				m_DxgiFactory;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12Device>				m_Device;
	ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<ID3D12RootSignature>			m_RootSignature;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;
};
