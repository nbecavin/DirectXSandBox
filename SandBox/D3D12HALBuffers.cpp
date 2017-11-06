
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
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		pInitData = &initData;
		initData.pSysMem = _Datas;
	}
	else
	{
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
	}
	
	D3D12_HEAP_PROPERTIES

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res)));

}

void D3D12IndexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	/*
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetDevice();
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

	// Create the vertex buffer.
	hr = Device->CreateBuffer(&bufferDesc, pInitData, &res);
	ASSERT(hr == S_OK);
	*/
}

bool D3D12VertexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, U32 Flags)
{
	/*
	ID3D12GraphicsCommandList * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData != NULL;
	*/
	return FALSE;
}

bool D3D12IndexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, U32 Flags)
{
	/*
	ID3D12GraphicsCommandList * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData != NULL;
	*/
	return FALSE;
}

void D3D12VertexBuffer::Unlock()
{
	///res->Unmap(0);
}

void D3D12IndexBuffer::Unlock()
{
	//res->Unmap( 0);
}

#endif
