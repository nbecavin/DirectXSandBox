
#if defined(_PCDX12)
#include <DXRenderer.h>
#include <D3D12HAL.h>

static const int sConstantBufferSize = 16 * 1024 * 1024;

void D3D12ConstantBuffer::Init()
{
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetDevice();

	//
	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sConstantBufferSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&m_CB));

	m_RingConfig.m_BeginPtr = m_CB->GetGPUVirtualAddress();
	m_RingConfig.m_WritePtr = m_RingConfig.m_BeginPtr;
	m_RingConfig.m_EndPtr = m_RingConfig.m_BeginPtr + sConstantBufferSize;
}

#endif
