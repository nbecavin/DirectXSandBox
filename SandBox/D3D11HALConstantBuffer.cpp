#include <DXRenderer.h>
#include <D3D11HALBuffers.h>

ConstantBuffer* D3D11HAL::CreateConstantBuffer(U32 _Size)
{
	D3D11ConstantBuffer* cb = new D3D11ConstantBuffer();
	MESSAGE("Begin create CB");
	cb->Create(_Size);
	MESSAGE("End create CB");
	return (cb->IsInited()) ? cb : nullptr;
}

void D3D11ConstantBuffer::Create(U32 _Size)
{
	HRESULT hr;
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetD3D11HAL().GetDevice();

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;

	/*
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
	*/

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = _Size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	Size = bufferDesc.ByteWidth;

	// Create the vertex buffer.
	hr = Device->CreateBuffer(&bufferDesc, nullptr, &res);
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

bool D3D11ConstantBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetD3D11HAL().GetImmediateDeviceContext();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res, 0, D3D11_MAP_WRITE_DISCARD, 0, &pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData != NULL;
}

void D3D11ConstantBuffer::Unlock()
{
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetD3D11HAL().GetImmediateDeviceContext();
	DeviceContext->Unmap(res, 0);
}
