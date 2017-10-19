#pragma once

using namespace Microsoft::WRL;

class D3D12HAL
{
public:
	void Init(int sizeX,int sizeY, sys::Renderer* owner);
	void Shut();

	void PresentFrame();

	ID3D12Device* GetDevice() { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList.Get(); }

	D3D12_CPU_DESCRIPTOR_HANDLE& GetCurrentBackBufferView() {
		return m_RenderTargetsView[m_FrameIndex];
	}

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
	ComPtr<ID3D12Fence>					m_SyncFence;

	UINT								m_FrameIndex = 0;
	static const UINT					m_BufferCount = 2;
	UINT								m_FrameCount = 1;

	// je ne sais pas ce que c'est pour l'instant
	ComPtr<ID3D12DescriptorHeap>		m_RtvHeap;
	UINT								m_RtvDescriptorSize;
	ComPtr<ID3D12Resource>				m_RenderTargets[m_BufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RenderTargetsView[m_BufferCount];
};
