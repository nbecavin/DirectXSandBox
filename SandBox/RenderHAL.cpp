#if defined(_PC)

#include <RenderHAL.h>
#include <Material.h>
#include <Bitmap.h>
#include "SandBox.h"
#include <WinMain.h>
#include <..\..\Shaders\ShaderRegs.h>
#include <ShaderConstants.h>
#include <RenderGraph.h>
#include <imgui.h>

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_AGILITYSDK_VERSION; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }

namespace sys {

	void RenderHAL::MainLoop()
	{
	}

	void RenderHAL::SetAccelerationStructure(U32 Slot, EShaderType Type, AccelerationStructure* AS)
	{
	}

	void RenderHAL::SetShaderResource(U32 Slot, EShaderType Type, Bitmap* Texture)
	{
	}

	void RenderHAL::SetUAV(U32 Slot, Bitmap* Texture)
	{
	}

	void RenderHAL::InitShaders()
	{
	}

	void RenderHAL::ReleaseAllResources()
	{
	}

	void RenderHAL::FullScreenQuad(Vec2f scale,Vec2f offset)
	{
		SetVertexDeclaration(m_ScreenVertexDecl);
		SetStreamSource(0,m_FullscreenQuadVB,0,32);

		/*
		XMVECTOR * _vec = (XMVECTOR*)(m_VSConstantCache);
		_vec[USER_CST+0] = XMVectorSet(scale.x,scale.y,offset.x,offset.y);
		UpdateVSConstants();
		*/

		SetPrimitiveTopology(PRIM_TRIANGLESTRIP);
		DrawInstanced(4,1,0,0);
	}

	void RenderHAL::PostProcess()
	{
	}

};

#endif //_PC

