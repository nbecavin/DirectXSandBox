#if defined(_PC)

#include <DXRenderer.h>

void DXVertexBuffer::Create(U32 _Size,U32 _Usage,void * _Datas)
{
#ifdef _PCDX11
	HRESULT hr;
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetDevice();

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_SUBRESOURCE_DATA * pInitData = NULL;
	if(_Datas)
	{
		bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth        = _Size;
		bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags   = 0;
		bufferDesc.MiscFlags        = 0;

		pInitData = &initData;
        initData.pSysMem = _Datas;
	}
	else
	{
		bufferDesc.Usage            = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth        = _Size;
		bufferDesc.BindFlags        = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags        = 0;
	}

	// Create the vertex buffer.
	hr = Device->CreateBuffer( &bufferDesc, pInitData, &res );
#endif
}

void DXIndexBuffer::Create(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas)
{
#ifdef _PCDX11
	HRESULT hr;
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetDevice();
	U32 _ItemSize = (_Fmt==FMT_IDX_16) ? 2 : 4;

	// Fill in a buffer description.
	D3D11_BUFFER_DESC bufferDesc;
	D3D11_SUBRESOURCE_DATA initData;
	D3D11_SUBRESOURCE_DATA * pInitData = NULL;
	if(_Datas)
	{
		bufferDesc.Usage            = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth        = _Size;
		bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags   = 0;
		bufferDesc.MiscFlags        = 0;

		pInitData = &initData;
        initData.pSysMem = _Datas;
	}
	else
	{
		bufferDesc.Usage            = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth        = _Size;
		bufferDesc.BindFlags        = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags   = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags        = 0;
	}

	// Create the vertex buffer.
	hr = Device->CreateBuffer( &bufferDesc, pInitData, &res );
	ASSERT(hr==S_OK);
#endif
}

bool DXVertexBuffer::Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags)
{
#ifdef _PCDX11
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res,0,D3D11_MAP_WRITE_NO_OVERWRITE,0,&pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData!=NULL;
#else
	return FALSE;
#endif
}

bool DXIndexBuffer::Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags)
{
#ifdef _PCDX11
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetDeviceContext();
	D3D11_MAPPED_SUBRESOURCE pMappedResource;
	DeviceContext->Map(res,0,D3D11_MAP_WRITE_NO_OVERWRITE,0,&pMappedResource);
	*pData = pMappedResource.pData;
	return pMappedResource.pData!=NULL;
#else
	return FALSE;
#endif
}

void DXVertexBuffer::Unlock()
{
#ifdef _PCDX11
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetDeviceContext();
	DeviceContext->Unmap(res,0);
#endif
}

void DXIndexBuffer::Unlock()
{
#ifdef _PCDX11
	ID3D11DeviceContext * DeviceContext = GET_RDR_INSTANCE()->GetDeviceContext();
	DeviceContext->Unmap(res,0);
#endif
}

#define SET_D3DDECL_END(_PTR_,_ID_,_Stream,_Offset,_Type,_Method,_Usage,_UsageIndex) \
		_PTR_[_ID].Stream = _Stream; \
		_PTR_[_ID].Offset = _Offset; \
		_PTR_[_ID].Type = _Type; \
		_PTR_[_ID].Method = _Method; \
		_PTR_[_ID].Usage = _Usage; \
		_PTR_[_ID].UsageIndex = _UsageIndex;

void DXVertexDeclaration::Create(VertexElement *Decl,U32 _ShaderUID)
{
#ifdef _PCDX11
	ID3D11Device * Device = GET_RDR_INSTANCE()->GetDevice();

	D3D11_INPUT_ELEMENT_DESC pElts[32];
	int nCurElt = 0;
	while(Decl[nCurElt].Type!=-1)
	{
		pElts[nCurElt].InputSlot = static_cast<WORD>(Decl[nCurElt].StreamId);
		pElts[nCurElt].AlignedByteOffset = static_cast<WORD>(Decl[nCurElt].Offset);
		pElts[nCurElt].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		pElts[nCurElt].InstanceDataStepRate = 0;
		switch(Decl[nCurElt].Type){
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
		switch(Decl[nCurElt].Semantic){
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
		case DECL_POSITION:
		default:
			pElts[nCurElt].SemanticName = "POSITION";
			pElts[nCurElt].SemanticIndex = 0;
			break;
		};
		nCurElt++;
	}
	ID3DBlob * pCode = GET_RDR_INSTANCE()->GetShaderBlob(_ShaderUID);
	Device->CreateInputLayout(pElts,nCurElt,pCode->GetBufferPointer(),pCode->GetBufferSize(),&res);
#endif
}

#endif //_PC
