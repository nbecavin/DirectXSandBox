
#if defined(_PCDX12)
#include <DXRenderer.h>
#include <D3D12HALBuffers.h>

void D3D12VertexBuffer::Create(U32 _Size, U32 _Usage, void * _Datas)
{
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetDevice();

	// Fill in a buffer description.
	D3D12_HEAP_PROPERTIES heapProperties;
	D3D12_RESOURCE_DESC bufferDesc;
	//D3D11_BUFFER_DESC bufferDesc;
	//D3D11_SUBRESOURCE_DATA initData;
	//D3D11_SUBRESOURCE_DATA * pInitData = NULL;
	if (_Datas)
	{
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		//bufferDesc.ByteWidth = _Size;
		//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//bufferDesc.CPUAccessFlags = 0;
		//bufferDesc.MiscFlags = 0;

		//pInitData = &initData;
		//initData.pSysMem = _Datas;
	}
	else
	{
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		//bufferDesc.ByteWidth = _Size;
		//bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		//bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//bufferDesc.MiscFlags = 0;
	}
	
	Size = bufferDesc.ByteWidth;
	//D3D12_HEAP_PROPERTIES

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));

	m_BufferView.BufferLocation = res->GetGPUVirtualAddress();
	m_BufferView.StrideInBytes = 0;
	m_BufferView.SizeInBytes = _Size;
}

void D3D12IndexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetDevice();
	/*
	U32 _ItemSize = (_Fmt == FMT_IDX_16) ? 2 : 4;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_SUBRESOURCE_DATA * pInitData = NULL;
	if (_Datas)
	{
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		pInitData = &initData;
		initData.pSysMem = _Datas;
	}
	else
	{
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
	}
	*/

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));

	m_BufferView.BufferLocation = res->GetGPUVirtualAddress();
	m_BufferView.Format = (_Fmt == FMT_IDX_16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
	m_BufferView.SizeInBytes = _Size;
}

bool D3D12VertexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, U32 Flags)
{
	if (!res)
		return FALSE;

	res->Map(0, nullptr, pData);
	m_LockedRange.Begin = OffsetToLock;
	m_LockedRange.End = SizeToLock ? SizeToLock : m_BufferView.SizeInBytes;
	return pData[0] != NULL;
}

bool D3D12IndexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, U32 Flags)
{
	res->Map(0, nullptr, pData);
	m_LockedRange.Begin = OffsetToLock;
	m_LockedRange.End = SizeToLock ? SizeToLock : m_BufferView.SizeInBytes;
	return pData[0] != NULL;
}

void D3D12VertexBuffer::Unlock()
{
	res->Unmap(0, &m_LockedRange);
	///res->Unmap(0);
}

void D3D12IndexBuffer::Unlock()
{
	//res->Unmap( 0);
	res->Unmap(0, &m_LockedRange);
}

#endif
