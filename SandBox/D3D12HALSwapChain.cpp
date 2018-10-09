#include "stdafx.h"
#if defined(_PCDX12)
#include <Renderer.h>
#include <D3D12HAL.h>
#include <WinMain.h>

void D3D12HAL::Init(int sizeX, int sizeY, sys::Renderer* owner)
{
	HRESULT hr;

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

#if 1 //defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory));

	//ComPtr<IDXGIAdapter1> hardwareAdapter;
	//GetHardwareAdapter(factory.Get(), &hardwareAdapter);

	D3D12CreateDevice(
		NULL,//hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_Device)
	);

	// Describe and create the command queue.
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = m_BufferCount;
	sd.BufferDesc.Width = sizeX;
	sd.BufferDesc.Height = sizeY;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.OutputWindow = sys::pc::hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	
	ComPtr<IDXGISwapChain> swapChain;
	hr = m_DxgiFactory->CreateSwapChain(
		m_CommandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
		&sd,
		&swapChain
	);
	swapChain.As(&m_SwapChain);

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	
	// Create descriptor heaps.
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
		rtvHeapDesc.NumDescriptors = m_BufferCount;
		rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RtvHeap));

		m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Create frame resources.
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

		// Create a RTV for each frame.
		for (UINT n = 0; n < m_BufferCount; n++)
		{
			m_SwapChain->GetBuffer(n, IID_PPV_ARGS(&m_RenderTargets[n]));
			m_Device->CreateRenderTargetView(m_RenderTargets[n].Get(), nullptr, rtvHandle);
			m_RenderTargetsView[n] = rtvHandle;

			//rtvHandle.Offset(1, m_RtvDescriptorSize);
			rtvHandle.ptr += m_RtvDescriptorSize;
		}
	}

	// Command Allocator
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

	// Create an empty root signature.
	{
		//xone code
		//CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		//rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		
		D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		rootSignatureDesc.NumParameters = 0;
		rootSignatureDesc.NumStaticSamplers = 0;
		rootSignatureDesc.pParameters = nullptr;
		rootSignatureDesc.pStaticSamplers = nullptr;

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
		m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
	}

	// Create the command list.
	m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));

	// Command lists are created in the recording state, but there is nothing
	// to record yet. The main loop expects it to be closed, so close it now.
	m_CommandList->Close();

	// Prepare for rendering
	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);

	m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_SyncFence));

	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_RenderTargets[m_FrameIndex].Get();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CommandList->ResourceBarrier(1, &barrier);

	memset(&m_CurrentPSO, 0, sizeof(m_CurrentPSO));

	D3D12_RASTERIZER_DESC rs;
	rs.AntialiasedLineEnable = FALSE;
	rs.CullMode = D3D12_CULL_MODE_NONE;
	rs.DepthBias = 0.f;
	rs.DepthBiasClamp = 0.f;
	rs.DepthClipEnable = TRUE;
	rs.FillMode = D3D12_FILL_MODE_SOLID;
	rs.FrontCounterClockwise = FALSE;
	rs.MultisampleEnable = FALSE;
	rs.SlopeScaledDepthBias = 0.f;
	rs.ForcedSampleCount = 1;
	rs.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	m_CurrentPSO.RasterizerState = rs;

	m_CurrentPSO.pRootSignature = m_RootSignature.Get();

	m_GlobalConstantBuffer.Init();

	OutputDebugString("DX12 Renderer is up and running");
}

void D3D12HAL::Shut()
{
	m_CommandList.Get()->Release();
	m_CommandQueue.Get()->Release();
	m_CommandAllocator.Get()->Release();
	m_Device.Get()->Release();
	m_SwapChain.Get()->Release();
}

void D3D12HAL::SetAndClearRenderTarget()
{
	m_CurrentPSO.NumRenderTargets = 1;
	m_CurrentPSO.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//m_CommandList->SetGra
	//GetCommandList()->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	GetCommandList()->ClearRenderTargetView(GetCurrentBackBufferView(), ClearColor, 0, nullptr);
	GetCommandList()->ClearDepthStencilView(m_DepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);
}

void D3D12HAL::PresentFrame()
{
	// Indicate that the back buffer will now be used to present.
	D3D12_RESOURCE_BARRIER barrierpresent;
	barrierpresent.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierpresent.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierpresent.Transition.pResource = m_RenderTargets[m_FrameIndex].Get();
	barrierpresent.Transition.Subresource = 0;
	barrierpresent.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierpresent.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	m_CommandList->ResourceBarrier(1, &barrierpresent);
	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	m_CommandList->Close();

	// Execute the command list.
	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// Present the frame.
	m_SwapChain->Present(1, 0);

	//WaitForPreviousFrame();
	{
		UINT frameCount = m_FrameCount;
		m_CommandQueue->Signal(m_SyncFence.Get(), frameCount);
		m_FrameCount++;

		while(m_SyncFence->GetCompletedValue() < frameCount)
		{
			//ThrowIfFailed(m_fence->SetEventOnCompletion(fence, m_fenceEvent));
			//WaitForSingleObject(m_fenceEvent, INFINITE);
			Sleep(1);
		}
	}

	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	m_CommandList->Reset(m_CommandAllocator.Get(), nullptr);
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = m_RenderTargets[m_FrameIndex].Get();
	barrier.Transition.Subresource = 0;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	m_CommandList->ResourceBarrier(1, &barrier);
	//m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
}

#endif
