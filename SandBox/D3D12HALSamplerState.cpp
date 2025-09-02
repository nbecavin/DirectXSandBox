#include <DXRenderer.h>
#include <D3D12HAL.h>

void D3D12SamplerState::Create(const SamplerDesc& Desc)
{
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();

	auto& heap = GET_RDR_INSTANCE()->GetD3D12HAL().GetSamplerHeap();
	U32 slot = heap.AllocateSlot(1);
	Device->CreateSampler(&Desc.desc, heap.GetCPUSlotHandle(slot));

	m_CpuBase = heap.GetCPUSlotHandle(slot);
}
