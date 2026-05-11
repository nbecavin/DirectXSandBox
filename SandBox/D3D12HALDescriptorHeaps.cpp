#include <D3D12HAL.h>
#include <Renderer.h>

void D3D12DescriptorHeap::Init(D3D12_DESCRIPTOR_HEAP_DESC& heapDesc, const char * name)
{
	ID3D12Device * Device = GetD3D12HALRef().GetDevice();
	HRESULT hr = Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_Heap));
	m_DescriptorSize = Device->GetDescriptorHandleIncrementSize(heapDesc.Type);
	m_CurrentDescriptorOffset = 0;
	m_Desc = heapDesc;
	m_CPUBase = m_Heap->GetCPUDescriptorHandleForHeapStart();
}

void D3D12GpuDescriptorHeap::Init(D3D12_DESCRIPTOR_HEAP_DESC& heapDesc, const char* name)
{
	ID3D12Device* Device = GetD3D12HALRef().GetDevice();
	HRESULT hr = Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_Heap));

	m_DescriptorSize = Device->GetDescriptorHandleIncrementSize(heapDesc.Type);

	m_Desc = heapDesc;
	m_CPUBase = m_Heap->GetCPUDescriptorHandleForHeapStart();
	if (heapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
		m_GPUBase = m_Heap->GetGPUDescriptorHandleForHeapStart();

	ResetDynamicSlotOffset();
	ResetStaticSlotOffset();
}
