#include <Renderer.h>
#include <DXRenderer.h>
#include <D3D12HAL.h>
#include <WinMain.h>

namespace D3D12ShaderUtils
{
	namespace StaticRootSignatureConstants
	{
		// Assume descriptors are volatile because we don't initialize all the descriptors in a table, just the ones used by the current shaders.
		const D3D12_DESCRIPTOR_RANGE_FLAGS SRVDescriptorRangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		const D3D12_DESCRIPTOR_RANGE_FLAGS CBVDescriptorRangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		const D3D12_DESCRIPTOR_RANGE_FLAGS UAVDescriptorRangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DATA_VOLATILE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
		const D3D12_DESCRIPTOR_RANGE_FLAGS SamplerDescriptorRangeFlags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
	}
}

D3D12_VERSIONED_ROOT_SIGNATURE_DESC& GetStaticGraphicsRootSignatureDesc()
{
	static const U32 DescriptorTableCount = 16;
	static struct
	{
		D3D12_SHADER_VISIBILITY Vis;
		D3D12_DESCRIPTOR_RANGE_TYPE Type;
		U32 Count;
		U32 BaseShaderReg;
		D3D12_DESCRIPTOR_RANGE_FLAGS Flags;
	} RangeDesc[DescriptorTableCount] =
	{
		{ D3D12_SHADER_VISIBILITY_PIXEL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, MAX_SRVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SRVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_PIXEL, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, MAX_CBS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::CBVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_PIXEL, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, MAX_SAMPLERS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SamplerDescriptorRangeFlags },

		{ D3D12_SHADER_VISIBILITY_VERTEX, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, MAX_SRVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SRVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_VERTEX, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, MAX_CBS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::CBVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_VERTEX, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, MAX_SAMPLERS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SamplerDescriptorRangeFlags },

		{ D3D12_SHADER_VISIBILITY_GEOMETRY, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, MAX_SRVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SRVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_GEOMETRY, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, MAX_CBS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::CBVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_GEOMETRY, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, MAX_SAMPLERS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SamplerDescriptorRangeFlags },

		{ D3D12_SHADER_VISIBILITY_HULL, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, MAX_SRVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SRVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_HULL, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, MAX_CBS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::CBVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_HULL, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, MAX_SAMPLERS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SamplerDescriptorRangeFlags },

		{ D3D12_SHADER_VISIBILITY_DOMAIN, D3D12_DESCRIPTOR_RANGE_TYPE_SRV, MAX_SRVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SRVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_DOMAIN, D3D12_DESCRIPTOR_RANGE_TYPE_CBV, MAX_CBS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::CBVDescriptorRangeFlags },
		{ D3D12_SHADER_VISIBILITY_DOMAIN, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, MAX_SAMPLERS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::SamplerDescriptorRangeFlags },

		{ D3D12_SHADER_VISIBILITY_ALL, D3D12_DESCRIPTOR_RANGE_TYPE_UAV, MAX_UAVS, 0, D3D12ShaderUtils::StaticRootSignatureConstants::UAVDescriptorRangeFlags },
	};

	static CD3DX12_ROOT_PARAMETER1 TableSlots[DescriptorTableCount];
	static CD3DX12_DESCRIPTOR_RANGE1 DescriptorRanges[DescriptorTableCount];

	for (U32 i = 0; i < DescriptorTableCount; i++)
	{
		DescriptorRanges[i].Init(
			RangeDesc[i].Type,
			RangeDesc[i].Count,
			RangeDesc[i].BaseShaderReg,
			0u,
			RangeDesc[i].Flags
		);

		TableSlots[i].InitAsDescriptorTable(1, &DescriptorRanges[i], RangeDesc[i].Vis);
	}

	static CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC RootDesc(DescriptorTableCount, TableSlots, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	return RootDesc;
}

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

#if 0 //defined(_DEBUG)
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
		ComPtr<ID3D12Debug1> debugController1;
		if (SUCCEEDED(debugController->QueryInterface(IID_PPV_ARGS(&debugController1))))
		{
			debugController1->EnableDebugLayer();
			debugController1->SetEnableGPUBasedValidation(true);
		}
	}
#endif

	hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_DxgiFactory));

	//ComPtr<IDXGIAdapter1> hardwareAdapter;
	//GetHardwareAdapter(factory.Get(), &hardwareAdapter);

	D3D12CreateDevice(
		NULL,//hardwareAdapter.Get(),
		D3D_FEATURE_LEVEL_12_1,
		IID_PPV_ARGS(&m_Device)
	);

	ComPtr<ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&infoQueue))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, false);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	}

	ComPtr<ID3D12DebugDevice1> debugDevice;
	if (SUCCEEDED(m_Device->QueryInterface(IID_PPV_ARGS(&debugDevice))))
	{
		DWORD option = D3D12_DEBUG_FEATURE_CONSERVATIVE_RESOURCE_STATE_TRACKING;
		debugDevice->SetDebugParameter(D3D12_DEBUG_DEVICE_PARAMETER_FEATURE_FLAGS, &option, sizeof(option));
	}

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

	{
		D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
		dsvHeapDesc.NumDescriptors = 1;
		dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DsvHeap));

		CD3DX12_RESOURCE_DESC resdesc;
		resdesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, sizeX, sizeY);
		resdesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE clearZ;
		clearZ.DepthStencil.Depth = 1;
		clearZ.DepthStencil.Stencil = 0;
		clearZ.Format = DXGI_FORMAT_D32_FLOAT;
		m_Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&resdesc,
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&clearZ,
			IID_PPV_ARGS(&m_DepthStencil));

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_DsvHeap->GetCPUDescriptorHandleForHeapStart());

		D3D12_DEPTH_STENCIL_VIEW_DESC desc;
		m_Device->CreateDepthStencilView(m_DepthStencil.Get(), nullptr, dsvHandle);
		m_DepthStencilView = dsvHandle;
	}

	// Create SRV descriptor heap
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 4096;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_SrvHeap.Init(heapDesc, "SRV/CBV Heap");
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = 2048;
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		m_SamplerHeap.Init(heapDesc, "Sampler Heap");
	}

	// Command Allocator
	m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator));

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

	// Create an empty root signature.
	{
		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

		// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

		//if (FAILED(m_Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
		{
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1;
		}

		CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
		ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 48, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC_WHILE_SET_AT_EXECUTE | D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
		ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 16, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

		CD3DX12_ROOT_PARAMETER1 rootParameters[2];
		rootParameters[0].InitAsDescriptorTable(1, &ranges[0]);// , D3D12_SHADER_VISIBILITY_PIXEL);
		rootParameters[1].InitAsDescriptorTable(1, &ranges[1]);//, D3D12_SHADER_VISIBILITY_PIXEL);

		CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		ComPtr<ID3DBlob> signature;
		ComPtr<ID3DBlob> error;
		HRESULT hr = 0;// D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
		
		// use the one from UE4 instead
		hr = D3DX12SerializeVersionedRootSignature(&GetStaticGraphicsRootSignatureDesc(), featureData.HighestVersion, &signature, &error);
		if (hr != S_OK)
		{
			char * errormessage = (char*)error->GetBufferPointer();
			MESSAGE(errormessage);
		}
		hr = m_Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature));
		if (hr != S_OK)
		{
			MESSAGE("ERROR : Failed to create Root Signature");
		}

		// Create dynamic descriptorHeaps
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};

		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NumDescriptors = 4000*(MAX_SRVS * 3 + MAX_CBS * 3);
		m_SRVDynamicHeap.Init(heapDesc, "Dynamic SRV Heap");

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc2 = {};
		heapDesc2.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		heapDesc2.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc2.NumDescriptors = 2048;
		m_SamplerDynamicHeap.Init(heapDesc2, "Dynamic Sampler Heap");
		
		for (int i = 0; i<_countof(m_CurrentSRV); i++)
		{
			for (int j = 0; j<_countof(m_CurrentSRV[i]); j++)
			{
				m_CurrentSRV[i][j] = {};
			}
		}		
		for (int i = 0; i<_countof(m_CurrentCBV); i++)
		{
			for (int j = 0; j<_countof(m_CurrentCBV[i]); j++)
			{
				m_CurrentCBV[i][j] = {};
			}
		}
		for (int i = 0; i < _countof(m_CurrentSampler); i++)
		{
			for (int j = 0; j < _countof(m_CurrentSampler[i]); j++)
			{
				m_CurrentSampler[i][j] = {};
			}
		}
	}

	OutputDebugString("DX12 Renderer is up and running\n");
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
	m_CurrentPSO.SampleDesc.Count = 1;
	m_CurrentPSO.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//m_CommandList->SetGra
	//GetCommandList()->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetCurrentBackBufferView();
	m_CommandList->OMSetRenderTargets(1, &rtv, false, &m_DepthStencilView);

	float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
	GetCommandList()->ClearRenderTargetView(GetCurrentBackBufferView(), ClearColor, 0, nullptr);
	GetCommandList()->ClearDepthStencilView(m_DepthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	ID3D12DescriptorHeap* ppHeaps[] = { m_SRVDynamicHeap.Get(), m_SamplerDynamicHeap.Get() };
	m_CommandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
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

	m_SRVDynamicHeap.SetSlot(0);
	m_SamplerDynamicHeap.SetSlot(0);
}
