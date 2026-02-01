#include <D3D12HAL.h>
#include <RenderHAL.h>
#include <Renderer.h>

static const int sConstantBufferSize = 16 * 1024 * 1024;

U32 Align(U32 uLocation, UINT uAlign)
{
	return ((uLocation + (U64)(uAlign - 1)) & ~((U64)(uAlign - 1)));
}

Buffer* D3D12HAL::CreateBuffer(U32 _Size)
{
	U32 alignedSize = Align(_Size, 256);

	ID3D12Device* Device = GetD3D12HALRef().GetDevice();
	D3D12Buffer* buffer = new D3D12Buffer(alignedSize);

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(alignedSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&buffer->res));

	return buffer;
}

ConstantBuffer* D3D12HAL::CreateConstantBuffer(U32 _Size)
{
	D3D12ConstantBuffer* cb = new D3D12ConstantBuffer();
	cb->Create(_Size);
	return (cb->IsInited()) ? cb : nullptr;
}

void D3D12ConstantBuffer::Create(U32 _Size)
{
	HRESULT hr;
	ID3D12Device * Device = GetD3D12HALRef().GetDevice();

	//
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(Align(_Size, 256)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));

	m_RingConfig.m_BeginPtr = res->GetGPUVirtualAddress();
	m_RingConfig.m_WritePtr = m_RingConfig.m_BeginPtr;
	m_RingConfig.m_EndPtr = m_RingConfig.m_BeginPtr + _Size;

	m_BufferView.BufferLocation = res->GetGPUVirtualAddress();
	m_BufferView.SizeInBytes = Align(_Size, 256);

	auto& heap = GetD3D12HALRef().GetSrvHeap();
	U32 slot = heap.AllocateSlot(1);
	Device->CreateConstantBufferView(&m_BufferView, heap.GetCPUSlotHandle(slot));

	m_CpuBase = heap.GetCPUSlotHandle(slot);
	m_GpuBase = heap.GetGPUSlotHandle(slot);
}

bool D3D12ConstantBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	res->Map(0, nullptr, pData);
	m_LockedRange.Begin = OffsetToLock;
	m_LockedRange.End = SizeToLock ? SizeToLock : m_BufferView.SizeInBytes;
	return pData[0] != NULL;
}

void D3D12ConstantBuffer::Unlock()
{
	res->Unmap(0, &m_LockedRange);
}
