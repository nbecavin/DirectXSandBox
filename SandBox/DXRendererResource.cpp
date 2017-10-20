#if defined(_PC)

#include <DXRenderer.h>
#include <WinMain.h>
#include <DXBuffers.h>
#include <Material.h>
#include <Bitmap.h>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
#define MessageBox(i,j,k,l)		OutputDebugStringA(j)
#endif

namespace sys {

	VertexBuffer*	DXRenderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void* _Datas)
	{
		DXVertexBuffer * buffer = m_VertexBufferDA.Add();
		MESSAGE("Begin create VB");
		buffer->Create(_Size,_Usage,_Datas);
		MESSAGE("End create VB");
		return (buffer->IsInited()) ? buffer : NULL;
	}

	IndexBuffer*	DXRenderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void* _Datas)
	{
		DXIndexBuffer * buffer = m_IndexBufferDA.Add();
		MESSAGE("Begin create IB");
		buffer->Create(_Size,_Usage,_Fmt,_Datas);
		MESSAGE("End create IB");
		return (buffer->IsInited()) ? buffer : NULL;
	}

	VertexDeclaration*	DXRenderer::CreateVertexDecl(VertexElement *Decl,U32 _ShaderUID)
	{
		DXVertexDeclaration * buffer = m_VertexDeclarationDA.Add();
		MESSAGE("Begin create Decl");
		buffer->Create(Decl,_ShaderUID);
		//return (buffer->IsInited()) ? buffer : NULL;
		MESSAGE("End create Decl");
		return buffer;
	}

	void DXRenderer::RegisterShaderFromSourceFile(U32 _ShaderUID,const char* src,const char* epoint)
	{
		HRESULT hr;

		char profile[32]="";
		char path[1024];

		sprintf(path,"..\\Shaders\\%s",src);
	
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));

		if(Type==SHADER_TYPE_VERTEX)
		{
			strcpy(profile,"vs_5_0");
		}
		if(Type==SHADER_TYPE_PIXEL)
		{
			strcpy(profile,"ps_5_0");
		}
		if(Type==SHADER_TYPE_COMPUTE)
		{
			strcpy(profile,"cs_5_0");
		}

		DWORD CpFlag = 0;//*D3DCOMPILE_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		//CpFlag |= D3DCOMPILE_DEBUG;
	#endif
		//CpFlag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
		CpFlag |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
		CpFlag |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

		D3D_SHADER_MACRO mac[128]; U32 macnb=0;
		mac[macnb].Name = "_PCDX11";
		mac[macnb].Definition = "1";
		macnb++;
#ifdef _PCDX12
		mac[macnb].Name = "_PCDX12";
		mac[macnb].Definition = "1";
		macnb++;
#endif
		mac[macnb].Name = 0;
		mac[macnb].Definition = 0;
		macnb++;

		ID3DBlob * pError = NULL;
		ID3DBlob * pCode = NULL;

		WCHAR wpath[2049];
		MultiByteToWideChar(CP_ACP,0,path,-1,wpath,2048);
		hr = D3DCompileFromFile(wpath,mac,D3D_COMPILE_STANDARD_FILE_INCLUDE,epoint,profile,CpFlag,0,&pCode,&pError);
		if(hr!=S_OK)
		{
			if(hr==D3D11_ERROR_FILE_NOT_FOUND)
				MessageBox(NULL,"File not found","Shader Error",MB_OK);
			else
			{
				char str[256];
				sprintf(str,"HR=%d\n",hr);
				MessageBox(NULL,str,"Shader Error",MB_OK);
			}
			if(pError)
				MessageBox(NULL,(char*)pError->GetBufferPointer(),"Shader Error",MB_OK);
		}
		if( pError )
			pError->Release();

		if(pCode)
		{
			GetHAL().CreateShaderResource(pCode, Type, SID);
		}
	}

	ID3DBlob * DXRenderer::GetShaderBlob(U32 _ShaderUID)
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));
		return GetHAL().GetShaderBlob(Type, SID);
	}

static UINT BitsPerPixel( DXGI_FORMAT fmt )
{
    switch( fmt )
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


static void GetSurfaceInfo( UINT width, UINT height, DXGI_FORMAT fmt, UINT* pNumBytes, UINT* pRowBytes, UINT* pNumRows )
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

    if( bc )
    {
        int numBlocksWide = 0;
        if( width > 0 )
            numBlocksWide = max( 1, width / 4 );
        int numBlocksHigh = 0;
        if( height > 0 )
            numBlocksHigh = max( 1, height / 4 );
        rowBytes = numBlocksWide * bcnumBytesPerBlock;
        numRows = numBlocksHigh;
    }
    else
    {
        UINT bpp = BitsPerPixel( fmt );
        rowBytes = ( width * bpp + 7 ) / 8; // round up to nearest byte
        numRows = height;
    }
    numBytes = rowBytes * numRows;
    if( pNumBytes != NULL )
        *pNumBytes = numBytes;
    if( pRowBytes != NULL )
        *pRowBytes = rowBytes;
    if( pNumRows != NULL )
        *pNumRows = numRows;
}

	static inline DXGI_FORMAT GetDXFormat(U32 Format,Bool sRGB=TRUE)
	{
		switch(Format){
		case BM_FORMAT_DXT1:
			return sRGB ? DXGI_FORMAT_BC1_UNORM_SRGB : DXGI_FORMAT_BC1_UNORM;
		case BM_FORMAT_DXT5:
			return sRGB ? DXGI_FORMAT_BC7_UNORM_SRGB : DXGI_FORMAT_BC7_UNORM;
		case BM_FORMAT_R32F:
			return sRGB ? DXGI_FORMAT_R32_FLOAT : DXGI_FORMAT_R32_FLOAT;
		case BM_FORMAT_ABGR8888:
		default:
			return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
		}
	}

	void DXRenderer::CreateTexture(Bitmap * _Bm)
	{
#ifdef _PCDX11
		MESSAGE("Create texture");

		bool sRGB = (_Bm->GetFlags()&BM_SRGB) ? true : false;

		D3D11_TEXTURE2D_DESC desc;
		memset(&desc,0,sizeof(desc));
		desc.Width = _Bm->GetSx();
		desc.Height = _Bm->GetSy();
		desc.ArraySize = _Bm->GetSz();
		desc.Format = GetDXFormat(_Bm->GetFormat(),sRGB);
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = _Bm->GetMips();

		if(_Bm->GetType()&BM_TYPE_RT)
			desc.BindFlags |= D3D11_BIND_RENDER_TARGET;

		D3D11_SUBRESOURCE_DATA * pData = NULL;
		D3D11_SUBRESOURCE_DATA Data[32];
		ZeroMemory(Data,sizeof(Data));
		TextureLink	* tex = new TextureLink;
		tex->Resource = NULL;
		tex->ShaderView = NULL;
		tex->Surface = NULL;

		//if(desc.Width != desc.Height)
		//	return;

		// Creature DX resource from Bitmap
		if(_Bm->GetDatas())
		{
			BYTE* pSrcBits = _Bm->GetDatas();
			UINT NumBytes = 0;
			UINT RowBytes = 0;
			UINT NumRows = 0;
			U32 index = 0;
			for( U32 j = 0; j < desc.ArraySize; j++ )
			{
				UINT w = desc.Width;
				UINT h = desc.Height;
				for( U32 i = 0; i < _Bm->GetMips(); i++ )
				{
					GetSurfaceInfo( w, h, desc.Format, &NumBytes, &RowBytes, &NumRows );
					Data[index].pSysMem = ( void* )pSrcBits;
					Data[index].SysMemPitch = RowBytes;
					Data[index].SysMemSlicePitch = 0;
					++index;
					
					pSrcBits += NumBytes;
					w = w >> 1;
					h = h >> 1;
					if( w == 0 )
						w = 1;
					if( h == 0 )
						h = 1;
				}
			}

			desc.Usage = D3D11_USAGE_IMMUTABLE;
			pData = Data;
		}

		_Bm->BinHwResId((U32)tex);

		if(_Bm->GetType()&BM_TYPE_2D)
		{
			HRESULT hr = GetDevice()->CreateTexture2D(&desc,pData,&tex->Tex2D);
			if(hr!=S_OK)
			{
				MESSAGE("Failed create texture");
				return;
			}
		}

		{ // Create shader view
			D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
			ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
			SRVDesc.Format = desc.Format;
			SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			SRVDesc.Texture2D.MipLevels = desc.MipLevels;
			GetDevice()->CreateShaderResourceView(tex->Resource,&SRVDesc,&tex->ShaderView);
		}

		if(_Bm->GetType()&BM_TYPE_RT)
		{ // Create the render target view
			D3D11_RENDER_TARGET_VIEW_DESC DescRT;
			DescRT.Format = desc.Format;
			DescRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			DescRT.Texture2D.MipSlice = 0;
			GetDevice()->CreateRenderTargetView(tex->Resource,&DescRT,&tex->Surface);
		}

#endif
		MESSAGE("DX resource created");
	}

	ID3D11Buffer * CreateConstantBuffer(void* _DataPtr,U32 _DataSize)
	{
#ifdef _PCDX11
		ID3D11Buffer * pCstBuffer;

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = _DataSize;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = _DataPtr;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;
		((DXRenderer*)gData.Rdr)->GetDevice()->CreateBuffer(&desc,&initialData,&pCstBuffer);
		return pCstBuffer;
#else
		return NULL;
#endif
	}

};

//
// Constant buffers

void* CameraConstant::CreateHwRes()
{
	CameraConstant t;
	return (void*)sys::CreateConstantBuffer(&t,sizeof(t));
}

void* PostProcessConstant::CreateHwRes()
{
	PostProcessConstant t;
	return (void*)sys::CreateConstantBuffer(&t,sizeof(t));
}

#endif //_PC
