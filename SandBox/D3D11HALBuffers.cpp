#if defined(_PCDX11)
#include <DXRenderer.h>
#include <D3D11HALBuffers.h>

void D3D11VertexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	HRESULT hr;
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetDevice();

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_SUBRESOURCE_DATA * pInitData = NULL;
	if (_Datas)
	{
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
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
	}

	Size = bufferDesc.ByteWidth;

	// Create the vertex buffer.
	hr = Device->CreateBuffer(&bufferDesc, pInitData, &res);
}

void D3D11IndexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	HRESULT hr;
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetDevice();
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

	Size = bufferDesc.ByteWidth;

	// Create the vertex buffer.
	hr = Device->CreateBuffer(&bufferDesc, pInitData, &res);
	ASSERT(hr == S_OK);
}

static D3D11_MAP GetD3D11MapFromEMap(Buffer::EMap v)
{
	switch (v)
	{
	case Buffer::Read: return D3D11_MAP_READ;
	case Buffer::Write: return D3D11_MAP_WRITE;
	case Buffer::ReadWrite: return D3D11_MAP_READ_WRITE;
	case Buffer::WriteDiscard: return D3D11_MAP_WRITE_DISCARD;
	case Buffer::WriteNoOverwrite: return D3D11_MAP_WRITE_NO_OVERWRITE;
	default:
		return D3D11_MAP_WRITE_DISCARD;
	};
}

bool D3D11VertexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res, 0, GetD3D11MapFromEMap(Flags), 0, &pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData != NULL;
}

bool D3D11IndexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res, 0, GetD3D11MapFromEMap(Flags), 0, &pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData != NULL;
}

void D3D11VertexBuffer::Unlock()
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	DeviceContext->Unmap(res, 0);
}

void D3D11IndexBuffer::Unlock()
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetCommandList();
	DeviceContext->Unmap(res, 0);
}

#endif
