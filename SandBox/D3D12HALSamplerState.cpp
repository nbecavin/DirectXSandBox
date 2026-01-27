#include <D3D12HAL.h>
#include <RenderHAL.h>
#include <Renderer.h>

void D3D12SamplerState::Create(const SamplerDesc& Desc)
{
	ID3D12Device * Device = GetHALPtr<D3D12HAL>()->GetDevice();

	auto& heap = GetHALPtr<D3D12HAL>()->GetSamplerHeap();
	U32 slot = heap.AllocateSlot(1);
	Device->CreateSampler(&Desc.desc, heap.GetCPUSlotHandle(slot));

	m_CpuBase = heap.GetCPUSlotHandle(slot);
}
