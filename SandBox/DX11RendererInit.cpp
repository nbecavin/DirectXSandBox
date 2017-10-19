#if defined(_PC)

#include <DXRenderer.h>
#include <Material.h>
#include <Bitmap.h>
#include "SandBox.h"
#include <WinMain.h>
#include <..\..\Shaders\ShaderRegs.h>

#if WINAPI_FAMILY==WINAPI_FAMILY_APP
using namespace Windows::Graphics::Display;
// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float ConvertDipsToPixels(float dips)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}
#endif

namespace sys
{

	int DXRenderer::Init()
	{
		SizeX = 1280;
		SizeY = 720;

		GetHAL().Init(SizeX, SizeY, this);


#ifdef _PCDX11
		// Create a render target view
		ID3D11Texture2D* pBackBuffer = NULL;
		HRESULT hr = GetHAL().GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr))
			return hr;

		hr = GetDevice()->CreateRenderTargetView( pBackBuffer, NULL, &m_BackBuffer );
		pBackBuffer->Release();
		if( FAILED( hr ) )
			return hr;

		// Create depth buffer
		//hr = Device->CreateDepthStencilView( pDepthBuffer, 

		/*         texturefmt = DXGI_FORMAT_R24G8_TYPELESS ;
                SRVfmt = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
               DSVfmt = DXGI_FORMAT_D24_UNORM_S8_UINT;
			*/

        // Create depth stencil texture
        ID3D11Texture2D* pDepthStencil = NULL;
        D3D11_TEXTURE2D_DESC descDepth;
        descDepth.Width = SizeX;
        descDepth.Height = SizeY;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;//DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;
        hr = GetDevice()->CreateTexture2D( &descDepth, NULL, &pDepthStencil );
        if( FAILED( hr ) )
            return hr;

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		descDSV.Flags = 0;
        descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//descDepth.Format;
        if( descDepth.SampleDesc.Count > 1 )
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        else
            descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = GetDevice()->CreateDepthStencilView( pDepthStencil, &descDSV, &m_DepthBuffer );
        if( FAILED( hr ) )
            return hr;

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory( &SRVDesc, sizeof( SRVDesc ) );
		SRVDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;
		hr = GetDevice()->CreateShaderResourceView( pDepthStencil, &SRVDesc,&m_ZBuffer);
        if( FAILED( hr ) )
            return hr;

		GetDeviceContext()->OMSetRenderTargets( 1, &m_BackBuffer, m_DepthBuffer );

		// Fill in a buffer description.
		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = VS_CONSTANT_BUFFER_SIZE;
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		cbDesc.StructureByteStride = 0;
		hr = GetDevice()->CreateBuffer( &cbDesc, NULL, &m_VSConstant );

		D3D11_RASTERIZER_DESC rs;
		rs.AntialiasedLineEnable = FALSE;
		rs.CullMode = D3D11_CULL_NONE;
		rs.DepthBias = 0.f;
		rs.DepthBiasClamp = 0.f;
		rs.DepthClipEnable = TRUE;
		rs.FillMode = D3D11_FILL_SOLID;
		rs.FrontCounterClockwise = FALSE;
		rs.MultisampleEnable = FALSE;
		rs.ScissorEnable = FALSE;
		rs.SlopeScaledDepthBias = 0.f;
		GetDevice()->CreateRasterizerState(&rs,&m_DefaultRS);

		D3D11_DEPTH_STENCIL_DESC ds;
		ds.DepthEnable = TRUE;
		ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		ds.StencilEnable = FALSE;
		ds.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		ds.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		ds.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		ds.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		ds.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		ds.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		ds.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		ds.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		ds.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		GetDevice()->CreateDepthStencilState(&ds,&m_DefaultDS);

		ds.DepthEnable = FALSE;
		ds.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		ds.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		GetDevice()->CreateDepthStencilState(&ds,&m_DSS_NoZWrite);

		D3D11_SAMPLER_DESC ss;
		ZeroMemory( &ss, sizeof(ss) );
		ss.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		ss.AddressU = ss.AddressV = ss.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		ss.MaxAnisotropy = 1;
		ss.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		ss.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&ss,&m_DefaultSS);

		ZeroMemory( &ss, sizeof(ss) );
		ss.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		ss.AddressU = ss.AddressV = ss.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		ss.MaxAnisotropy = 1;
		ss.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		ss.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&ss,&m_NoBilinearSS);
#endif

		InitShaders();
		InitSurface();
		InitStaticDatas();
		return 0;
	}

	bool DXRenderer::InitStaticDatas()
	{
		if(!Renderer::InitStaticDatas())
			return FALSE;


#ifdef _PCDX11
		XMFLOAT3	lightDir;

		#define SH_ORDER		2
		float shX[SH_ORDER*SH_ORDER];
		float shY[SH_ORDER*SH_ORDER];
		float shZ[SH_ORDER*SH_ORDER];

		lightDir.x=0.f;
		lightDir.y=-1.f;
		lightDir.z=1.f;
		//D3DXSHEvalDirectionalLight(SH_ORDER,&lightDir,0,0,1,shX,shY,shZ);

		lightDir.x=0.f;
		lightDir.y=-1.f;
		lightDir.z=0.f;
		//D3DXSHEvalHemisphereLight(SH_ORDER,&lightDir,D3DXCOLOR(1,0,0,1),D3DXCOLOR(0,1,0,1),shX,shY,shZ);

		Vec4f	coeffs[16];
		for(int i=0;i<(SH_ORDER*SH_ORDER);i++)
		{
			coeffs[i].x = shX[i];
			coeffs[i].y = shY[i];
			coeffs[i].z = shZ[i];
			coeffs[i].w = 1.f;
		}

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof(coeffs);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA initialData;
		initialData.pSysMem = coeffs;
		initialData.SysMemPitch = 0;
		initialData.SysMemSlicePitch = 0;
		GetDevice()->CreateBuffer(&desc,&initialData,&m_SHHemisphere);

		/*
		CameraConstant temp;
		desc.ByteWidth = sizeof(CameraConstant);
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		initialData.pSysMem = &temp;
		Device->CreateBuffer(&desc,&initialData,&m_CameraConstant);
		*/
		m_CameraConstant = (ID3D11Buffer*)CameraConstant::CreateHwRes();
		m_PostProcessConstant = (ID3D11Buffer*)PostProcessConstant::CreateHwRes();

#endif
		return TRUE;
	}

};

#endif //_PC
