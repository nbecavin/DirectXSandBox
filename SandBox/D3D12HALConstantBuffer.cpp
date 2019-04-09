#include <DXRenderer.h>
#include <D3D12HAL.h>

static const int sConstantBufferSize = 16 * 1024 * 1024;

ConstantBuffer* D3D12HAL::CreateConstantBuffer(U32 _Size)
{
	D3D12ConstantBuffer* cb = new D3D12ConstantBuffer();
	MESSAGE("Begin create CB");
	cb->Create(_Size);
	MESSAGE("End create CB");
	return (cb->IsInited()) ? cb : nullptr;
}

void D3D12ConstantBuffer::Create(U32 _Size)
{
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();

	//
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));

	m_RingConfig.m_BeginPtr = res->GetGPUVirtualAddress();
	m_RingConfig.m_WritePtr = m_RingConfig.m_BeginPtr;
	m_RingConfig.m_EndPtr = m_RingConfig.m_BeginPtr + _Size;

	m_BufferView.BufferLocation = res->GetGPUVirtualAddress();
	m_BufferView.SizeInBytes = _Size;

	tex->m_D3D12SRVcpu.ptr = GET_RDR_INSTANCE()->GetD3D12HAL().m_SrvHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_CurrentSrvDescriptorOffset * m_SrvDescriptorSize;
	tex->m_D3D12SRVgpu.ptr = GET_RDR_INSTANCE()->GetD3D12HAL().GetSrvHeap()->GetGPUDescriptorHandleForHeapStart().ptr + m_CurrentSrvDescriptorOffset * m_SrvDescriptorSize;
	Device->CreateConstantBufferView(&m_BufferView, tex->m_D3D12SRVcpu);
	m_CurrentSrvDescriptorOffset++;

	//tex->m_D3D12SRVgpu.ptr = m_SrvHeap->GetGPUDescriptorHandleForHeapStart().ptr + m_CurrentSrvDescriptorOffset * m_SrvDescriptorSize;
	//Device->CreateShaderResourceView(tex->Resource12, &SRVDesc, tex->m_D3D12SRVcpu);
	//m_CurrentSrvDescriptorOffset++;
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
