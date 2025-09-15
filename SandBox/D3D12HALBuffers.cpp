#include <DXRenderer.h>
#include <D3D12HALBuffers.h>

VertexBuffer* D3D12HAL::CreateVertexBuffer(U32 _Size, U32 _Usage, void* _Datas)
{
	D3D12VertexBuffer* vb = new D3D12VertexBuffer();
	vb->Create(_Size, _Usage, 0, _Datas);
	return (vb->IsInited()) ? vb : nullptr;
}

IndexBuffer* D3D12HAL::CreateIndexBuffer(U32 _Size, U32 _Usage, U32 _Fmt, void* _Datas)
{
	D3D12IndexBuffer* ib = new D3D12IndexBuffer();
	ib->Create(_Size, _Usage, _Fmt, _Datas);
	return (ib->IsInited()) ? ib : nullptr;
}

void D3D12VertexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();

	// Fill in a buffer description.
	D3D12_HEAP_PROPERTIES heapProperties;
	D3D12_RESOURCE_DESC bufferDesc;
	if (_Datas)
	{
		//heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//bufferDesc.Usage = D3D12_USAGE_DEFAULT;
		//bufferDesc.ByteWidth = _Size;
		//bufferDesc.BindFlags = D3D12_BIND_VERTEX_BUFFER;
		//bufferDesc.CPUAccessFlags = 0;
		//bufferDesc.MiscFlags = 0;

		//pInitData = &initData;
		//initData.pSysMem = _Datas;
	}
	else
	{
		heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		//bufferDesc.Usage = D3D12_USAGE_DYNAMIC;
		//bufferDesc.ByteWidth = _Size;
		//bufferDesc.BindFlags = D3D12_BIND_VERTEX_BUFFER;
		//bufferDesc.CPUAccessFlags = D3D12_CPU_ACCESS_WRITE;
		//bufferDesc.MiscFlags = 0;
	}
	
	m_BufferView.StrideInBytes = 0;
	m_BufferView.SizeInBytes = _Size;
	Size = m_BufferView.SizeInBytes;
	//D3D12_HEAP_PROPERTIES

	Device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(_Size),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&res));

	m_BufferView.BufferLocation = res->GetGPUVirtualAddress();

	if (_Datas)
	{
		void * dst;
		Lock(0, 0, &dst);
		memcpy(dst, _Datas, m_BufferView.SizeInBytes);
		Unlock();
	}
}

void D3D12IndexBuffer::Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas)
{
	HRESULT hr;
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();
	/*
	U32 _ItemSize = (_Fmt == FMT_IDX_16) ? 2 : 4;

	// Fill in a buffer description.
	D3D12_BUFFER_DESC bufferDesc;
	D3D12_SUBRESOURCE_DATA initData;
	D3D12_SUBRESOURCE_DATA * pInitData = NULL;
	if (_Datas)
	{
		bufferDesc.Usage = D3D12_USAGE_DEFAULT;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D12_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		pInitData = &initData;
		initData.pSysMem = _Datas;
	}
	else
	{
		bufferDesc.Usage = D3D12_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = _Size;
		bufferDesc.BindFlags = D3D12_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D12_CPU_ACCESS_WRITE;
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

	if (_Datas)
	{
		void * dst;
		Lock(0, 0, &dst);
		memcpy(dst, _Datas, m_BufferView.SizeInBytes);
		Unlock();
	}
}

bool D3D12VertexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	res->Map(0, nullptr, pData);
	m_LockedRange.Begin = OffsetToLock;
	m_LockedRange.End = SizeToLock ? SizeToLock : m_BufferView.SizeInBytes;
	return pData[0] != NULL;
}

bool D3D12IndexBuffer::Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags)
{
	res->Map(0, nullptr, pData);
	m_LockedRange.Begin = OffsetToLock;
	m_LockedRange.End = SizeToLock ? SizeToLock : m_BufferView.SizeInBytes;
	return pData[0] != NULL;
}

void D3D12VertexBuffer::Unlock()
{
	res->Unmap(0, &m_LockedRange);
}

void D3D12IndexBuffer::Unlock()
{
	//res->Unmap( 0);
	res->Unmap(0, &m_LockedRange);
}

#define SET_D3DDECL_END(_PTR_,_ID_,_Stream,_Offset,_Type,_Method,_Usage,_UsageIndex) \
		_PTR_[_ID].Stream = _Stream; \
		_PTR_[_ID].Offset = _Offset; \
		_PTR_[_ID].Type = _Type; \
		_PTR_[_ID].Method = _Method; \
		_PTR_[_ID].Usage = _Usage; \
		_PTR_[_ID].UsageIndex = _UsageIndex;

void D3D12VertexDeclaration::Create(VertexElement *Decl)
{
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();

	int nCurElt = 0;
	while (Decl[nCurElt].Type != -1)
	{
		pElts[nCurElt].InputSlot = static_cast<WORD>(Decl[nCurElt].StreamId);
		pElts[nCurElt].AlignedByteOffset = static_cast<WORD>(Decl[nCurElt].Offset);
		pElts[nCurElt].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		pElts[nCurElt].InstanceDataStepRate = 0;
		switch (Decl[nCurElt].Type) {
		case DECL_FMT_COLOR:	pElts[nCurElt].Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case DECL_FMT_UBYTE4:	pElts[nCurElt].Format = DXGI_FORMAT_R8G8B8A8_UINT; break;
		case DECL_FMT_SHORT2:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16_UINT; break;
		case DECL_FMT_SHORT4:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16B16A16_UINT; break;
		case DECL_FMT_UBYTE4N:	pElts[nCurElt].Format = DXGI_FORMAT_R8G8B8A8_UNORM; break;
		case DECL_FMT_SHORT2N:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16_SNORM; break;
		case DECL_FMT_SHORT4N:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16B16A16_SNORM; break;
		case DECL_FMT_USHORT2N:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16_UNORM; break;
		case DECL_FMT_USHORT4N:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16B16A16_UNORM; break;
		case DECL_FMT_UDEC3:	pElts[nCurElt].Format = DXGI_FORMAT_R10G10B10A2_UINT; break;
		case DECL_FMT_DEC3N:	pElts[nCurElt].Format = DXGI_FORMAT_R10G10B10A2_UNORM; break;
		case DECL_FMT_FLOAT16_2:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16_FLOAT; break;
		case DECL_FMT_FLOAT16_4:	pElts[nCurElt].Format = DXGI_FORMAT_R16G16B16A16_FLOAT; break;
		case DECL_FMT_FLOAT1:	pElts[nCurElt].Format = DXGI_FORMAT_R32_FLOAT; break;
		case DECL_FMT_FLOAT2:	pElts[nCurElt].Format = DXGI_FORMAT_R32G32_FLOAT; break;
		case DECL_FMT_FLOAT3:	pElts[nCurElt].Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
		case DECL_FMT_FLOAT4:	pElts[nCurElt].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
		default:
			MESSAGE("Unknown decl type");
			break;
		}
		switch (Decl[nCurElt].Semantic) {
		case DECL_TEXCOORD0:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		case DECL_TEXCOORD1:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 1;
			break;
		case DECL_TEXCOORD2:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 2;
			break;
		case DECL_TEXCOORD3:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 3;
			break;
		case DECL_TEXCOORD4:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 4;
			break;
		case DECL_TEXCOORD5:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 5;
			break;
		case DECL_TEXCOORD6:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 6;
			break;
		case DECL_TEXCOORD7:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 7;
			break;
		case DECL_TEXCOORD8:
			pElts[nCurElt].SemanticName = "TEXCOORD";
			pElts[nCurElt].SemanticIndex = 8;
			break;
		case DECL_NORMAL:
			pElts[nCurElt].SemanticName = "NORMAL";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		case DECL_TANGENT:
			pElts[nCurElt].SemanticName = "TANGENT";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		case DECL_BINORMAL:
			pElts[nCurElt].SemanticName = "BINORMAL";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		case DECL_COLOR0:
			pElts[nCurElt].SemanticName = "COLOR";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		case DECL_POSITION:
		default:
			pElts[nCurElt].SemanticName = "POSITION";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		};
		nCurElt++;
		if (nCurElt >= _countof(pElts))
			MESSAGE("Overflow!");
	}
	res.pInputElementDescs = pElts;
	res.NumElements = nCurElt;
}
