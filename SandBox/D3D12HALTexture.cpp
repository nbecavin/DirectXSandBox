#include <DXRenderer.h>
#include <D3D12HAL.h>
#include <Bitmap.h>

static inline DXGI_FORMAT GetDXFormat(BitmapFormat Format, Bool sRGB = TRUE)
{
	switch (Format) {
	case BM_BC1_UNORM:
		return sRGB ? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM;
	case BM_BC7_UNORM:
		return sRGB ? DXGI_FORMAT_BC7_UNORM_SRGB : DXGI_FORMAT_BC7_UNORM;
	case BM_R32_FLOAT:
		return sRGB ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R32_FLOAT;
	case BM_R8G8B8A8_UNORM:
		return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	default:
		return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
	}
}

static UINT BitsPerPixel(DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

	default:
		return 0;
	}
}

static void GetSurfaceInfo(UINT width, UINT height, DXGI_FORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows)
{
	UINT numBytes = 0;
	UINT rowBytes = 0;
	UINT numRows = 0;

	bool bc = true;
	int bcnumBytesPerBlock = 16;
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bcnumBytesPerBlock = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		break;

	default:
		bc = false;
		break;
	}

	if (bc)
	{
		int numBlocksWide = 0;
		if (width > 0)
			numBlocksWide = max(1, width / 4);
		int numBlocksHigh = 0;
		if (height > 0)
			numBlocksHigh = max(1, height / 4);
		rowBytes = numBlocksWide * bcnumBytesPerBlock;
		numRows = numBlocksHigh;
	}
	else
	{
		UINT bpp = BitsPerPixel(fmt);
		rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
	}
	numBytes = rowBytes * numRows;
	if (pNumBytes != NULL)
		*pNumBytes = numBytes;
	if (pRowBytes != NULL)
		*pRowBytes = rowBytes;
	if (pNumRows != NULL)
		*pNumRows = numRows;
}

U64 Align(U64 uLocation, UINT uAlign)
{
	//if ((0 == uAlign) || (uAlign & (uAlign - 1)))
	{
		//ThrowException("non-pow2 alignment");
	}

	return ((uLocation + (U64)(uAlign - 1)) & ~((U64)(uAlign - 1)));
}

void D3D12HAL::CreateTexture(Bitmap * _Bm)
{
	ID3D12Device * Device = GET_RDR_INSTANCE()->GetD3D12HAL().GetDevice();
	ID3D12GraphicsCommandList * pCommandList = GET_RDR_INSTANCE()->GetD3D12HAL().GetCommandList();

	bool sRGB = (_Bm->GetFlags()&BM_SRGB) ? true : false;

	CD3DX12_RESOURCE_DESC desc;
	desc = CD3DX12_RESOURCE_DESC::Tex2D(
		GetDXFormat(_Bm->GetFormat(), sRGB),
		_Bm->GetSx(),
		_Bm->GetSy(),
		_Bm->GetSz(),
		_Bm->GetMips(),
		1,
		0,
		D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT_UNKNOWN,
		0
	);

	if (_Bm->GetType()&BM_TYPE_RT)
	{
		desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	}

	sys::TextureLink * tex = new sys::TextureLink;
	tex->Resource12 = NULL;

	//if(desc.Width != desc.Height)
	//	return;

	_Bm->BinHwResId((U64)tex);

	if (_Bm->GetType()&BM_TYPE_2D)
	{
		HRESULT hr = Device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&desc,
			/*D3D12_RESOURCE_STATE_GENERIC_READ|*/ D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&tex->Resource12));
		if (hr != S_OK)
		{
			MESSAGE("Failed create texture");
			return;
		}

		// Fill in resource with initial data
		if (_Bm->GetDatas())
		{
			ID3D12Resource * m_spUploadBuffer;
			UINT8* m_pDataBegin = nullptr;    // starting position of upload buffer
			UINT8* m_pDataCur = nullptr;      // current position of upload buffer
			UINT8* m_pDataNext = nullptr;      // next position of upload buffer
			UINT8* m_pDataEnd = nullptr;      // ending position of upload buffer

			DWORD uSize = GetRequiredIntermediateSize(tex->Resource12, 0, _Bm->GetMips());
			//uSize = 16*1024*1024;

			HRESULT hr = Device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(uSize),
				D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
				IID_PPV_ARGS(&m_spUploadBuffer));
			if (SUCCEEDED(hr))
			{
				void* pData;
				//
				// No CPU reads will be done from the resource.
				//
				CD3DX12_RANGE readRange(0, 0);
				m_spUploadBuffer->Map(0, &readRange, &pData);
				m_pDataCur = m_pDataBegin = reinterpret_cast<UINT8*>(pData);
				m_pDataNext = m_pDataCur;
				m_pDataEnd = m_pDataBegin + uSize;
			}
			else{
				__debugbreak();
			}

			//
			// Sub-allocate from the buffer, with offset aligned.
			//

			auto SuballocateFromBuffer = [&](SIZE_T _uSize, UINT _uAlign)
			{
				m_pDataCur = reinterpret_cast<UINT8*>(
					Align(reinterpret_cast<SIZE_T>(m_pDataNext), _uAlign)
					);
				//m_pDataNext = m_pDataCur + _uSize;
				return ((m_pDataCur + _uSize) > m_pDataEnd) ? E_INVALIDARG : S_OK;
			};

			BYTE* pSrcBits = _Bm->GetDatas();
			UINT NumBytes = 0;
			UINT RowBytes = 0;
			UINT NumRows = 0;
			for (U32 j = 0; j < 1/*desc.DepthOrArraySize*/; j++)
			{
				for (U32 i = 0; i < _Bm->GetMips(); i++)
				{
					UINT w = Max<UINT>(1, desc.Width >> i);
					UINT h = Max<UINT>(1, desc.Height >> i);

					GetSurfaceInfo(w, h, desc.Format, &NumBytes, &RowBytes, &NumRows);

					D3D12_SUBRESOURCE_FOOTPRINT pitchedDesc = { 0 };
					pitchedDesc.Format = desc.Format;
					pitchedDesc.Width = w;
					pitchedDesc.Height = h;
					pitchedDesc.Depth = 1;
					pitchedDesc.RowPitch = Align(RowBytes, D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

					int ret = SuballocateFromBuffer(
						pitchedDesc.Height * pitchedDesc.RowPitch,
						D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT
					);

					if (ret == E_INVALIDARG)
					{
					//	MESSAGE("ret == E_INVALIDARG");
					}

					int SubResourceIndex = j * desc.MipLevels + i;

					D3D12_PLACED_SUBRESOURCE_FOOTPRINT _footprint;
					UINT _numRows;
					UINT64 _rowSizeInBytes, _totalBytes;

					D3D12_PLACED_SUBRESOURCE_FOOTPRINT placedTexture2D = { 0 };
					placedTexture2D.Offset = m_pDataCur - m_pDataBegin;
					placedTexture2D.Footprint = pitchedDesc;
					m_Device->GetCopyableFootprints(&desc, SubResourceIndex, 1, m_pDataCur - m_pDataBegin, &placedTexture2D, &_numRows, &_rowSizeInBytes, &_totalBytes);

					for (UINT y = 0; y < NumRows; y++)
					{
						UINT8 *pScan = m_pDataBegin + placedTexture2D.Offset + y * placedTexture2D.Footprint.RowPitch;
						UINT8 *pSrc = pSrcBits + y * RowBytes;
						memcpy(pScan, pSrc, RowBytes);
					}

					pCommandList->CopyTextureRegion(
						&CD3DX12_TEXTURE_COPY_LOCATION(tex->Resource12, SubResourceIndex),
						0, 0, 0,
						&CD3DX12_TEXTURE_COPY_LOCATION(m_spUploadBuffer, placedTexture2D),
						nullptr);

					pSrcBits += NumBytes;
				}
			}
		}
	}

	m_CommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(tex->Resource12, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE));

	{ // Create shader view
		D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;
		SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = desc.MipLevels;

		U32 slot = m_SrvHeap.AllocateSlot(1);
		tex->m_D3D12SRVcpu.ptr = m_SrvHeap.GetCPUSlotHandle(slot).ptr;
		tex->m_D3D12SRVgpu.ptr = m_SrvHeap.GetGPUSlotHandle(slot).ptr;
		Device->CreateShaderResourceView(tex->Resource12, &SRVDesc, tex->m_D3D12SRVcpu);
	}

	/*
	if (_Bm->GetType()&BM_TYPE_RT)
	{ // Create the render target view
		D3D12_RENDER_TARGET_VIEW_DESC DescRT;
		DescRT.Format = desc.Format;
		DescRT.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		DescRT.Texture2D.MipSlice = 0;
		GetDevice()->CreateRenderTargetView(tex->Resource, &DescRT, &tex->Surface);
	}
	*/
}
