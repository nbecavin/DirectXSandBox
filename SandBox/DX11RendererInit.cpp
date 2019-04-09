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

		//// Fill in a buffer description.
		//D3D11_BUFFER_DESC cbDesc;
		//cbDesc.ByteWidth = VS_CONSTANT_BUFFER_SIZE;
		//cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		//cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//cbDesc.MiscFlags = 0;
		//cbDesc.StructureByteStride = 0;
		//hr = GetDevice()->CreateBuffer( &cbDesc, NULL, &m_VSConstant );

		m_DefaultRS.desc.AntialiasedLineEnable = FALSE;
		m_DefaultRS.desc.CullMode = D3D11_CULL_NONE;
		m_DefaultRS.desc.DepthBias = 0.f;
		m_DefaultRS.desc.DepthBiasClamp = 0.f;
		m_DefaultRS.desc.DepthClipEnable = TRUE;
		m_DefaultRS.desc.FillMode = D3D11_FILL_SOLID;
		m_DefaultRS.desc.FrontCounterClockwise = FALSE;
		m_DefaultRS.desc.MultisampleEnable = FALSE;
		m_DefaultRS.desc.ScissorEnable = FALSE;
		m_DefaultRS.desc.SlopeScaledDepthBias = 0.f;

		m_DefaultDS.desc.DepthEnable = TRUE;
		m_DefaultDS.desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		m_DefaultDS.desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		m_DefaultDS.desc.StencilEnable = FALSE;
		m_DefaultDS.desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		m_DefaultDS.desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		m_DefaultDS.desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		m_DefaultDS.desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		m_DefaultDS.desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		m_DSS_NoZWrite.desc.DepthEnable = FALSE;
		m_DSS_NoZWrite.desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		m_DSS_NoZWrite.desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

		m_DefaultSS.desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		m_DefaultSS.desc.AddressU = m_DefaultSS.desc.AddressV = m_DefaultSS.desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		m_DefaultSS.desc.MaxAnisotropy = 1;
		m_DefaultSS.desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		m_DefaultSS.desc.MaxLOD = D3D11_FLOAT32_MAX;

		m_NoBilinearSS.desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		m_NoBilinearSS.desc.AddressU = m_NoBilinearSS.desc.AddressV = m_NoBilinearSS.desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		m_NoBilinearSS.desc.MaxAnisotropy = 1;
		m_NoBilinearSS.desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		m_NoBilinearSS.desc.MaxLOD = D3D11_FLOAT32_MAX;

		InitShaders();
		InitSurface();
		InitStaticDatas();
		InitImGUI();
		return 0;
	}

	bool DXRenderer::InitStaticDatas()
	{
		if(!Renderer::InitStaticDatas())
			return FALSE;

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
//		GetDevice()->CreateBuffer(&desc,&initialData,&m_SHHemisphere);

		m_CameraConstant = CreateConstantBuffer(sizeof(CameraConstant));
		m_GlobalConstant = CreateConstantBuffer(sizeof(GlobalConstant));

		return TRUE;
	}

};

#endif //_PC
