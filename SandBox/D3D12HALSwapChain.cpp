#include <Renderer.h>
#include <D3D12HAL.h>
#include <WinMain.h>

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

	m_CommandAllocator->Reset();

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

	m_SRVDynamicHeap.ResetDynamicSlotOffset();
	m_SamplerDynamicHeap.ResetDynamicSlotOffset();
}
