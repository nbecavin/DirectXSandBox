#if defined(_PC)

#include <DXRenderer.h>
#include <Material.h>
#include <Bitmap.h>
#include "SandBox.h"
#include <WinMain.h>
#include <..\..\Shaders\ShaderRegs.h>
#include <ShaderConstants.h>

extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = D3D12_AGILITYSDK_VERSION; }
extern "C" { __declspec(dllexport) extern const char* D3D12SDKPath = u8".\\D3D12\\"; }

// RTXDI
// https://github.com/NVIDIA-RTX/RTXDI/blob/main/Doc/Integration.md

#include <Rtxdi/RtxdiParameters.h>
#include <Rtxdi/DI/ReSTIRDI.h>
#include <Rtxdi/GI/ReSTIRGI.h>

namespace sys {

	rtxdi::ReSTIRDIContext* m_RtxDIcontext = nullptr;

	void DXRenderer::MainLoop()
	{
		ProfileBeginEventArgs(0, "Frame %d", m_FrameIndex);

		if(!m_RtxDIcontext)
		{
			rtxdi::ReSTIRDIStaticParameters params = {};
			params.RenderWidth = gData.Rdr->GetSizeX();
			params.RenderHeight = gData.Rdr->GetSizeY();
			m_RtxDIcontext = new rtxdi::ReSTIRDIContext(params);
		}

		ImGui::Begin("Stats");
		ImGui::Text("FPS %f", 1.f/GetDeltaTime());
		ImGui::Text("Time %fms", GetDeltaTime()*1000.f);
		ImGui::End();

		m_Camera->Update();

		HRESULT hr;

		GetHAL().SetAndClearRenderTarget();

		TextureLink* hdrtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
		GetHAL().GetCommandList()->OMSetRenderTargets(1, &hdrtex->m_RTV, true, &GetHAL().GetDSV());
		GetHAL().GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(hdrtex->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Setup the viewport
		D3D12_VIEWPORT vp;
		vp.Width = (FLOAT)SizeX;
		vp.Height = (FLOAT)SizeY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		GetHAL().SetViewports(vp);

		DepthStencilDesc ds;
		SetDepthStencilState(ds);
		RasterizerDesc rs;
		SetRasterizerState(rs);

		//une view et une proj de base
		XMMATRIX m, proj, view;
		XMVECTOR eye = XMVectorSet(0, 1, -5, 0);
		XMVECTOR at = XMVectorSet(0, 1, 0, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);
		view = XMMatrixLookAtRH(eye, at, up);

		view = *reinterpret_cast<XMMATRIX*>(&m_Camera->GetViewMatrix());

		proj = XMMatrixPerspectiveRH(.25, .25f / ((float)SizeX / (float)SizeY), 0.1, 1000);

		CameraConstant * camCst;
		m_CameraConstant->Lock(0, 0, (void**)&camCst);
		camCst->ProjMatrix = XMMatrixTranspose(proj);
		camCst->ViewMatrix = XMMatrixTranspose(view);
		camCst->EyeWorld = m_Camera->GetWorldPosition();
		XMVECTOR det;
		camCst->InvProjMatrix = XMMatrixTranspose(XMMatrixInverse(&det, proj));
		m_CameraConstant->Unlock();

		GlobalParameters* globalParam;
		m_GlobalConstant->Lock(0, 0, (void**)&globalParam);
		globalParam[0] = m_GlobalParams;
		m_GlobalConstant->Unlock();

		// No geometry shader

		//GetCommandList()->PSSetSamplers(0, 1, &m_DefaultSS);
		//GetCommandList()->PSSetSamplers(1, 1, &m_DefaultSS);
		//GetCommandList()->PSSetSamplers(2, 1, &m_DefaultSS);
		SetConstantBuffer(0, SHADER_TYPE_VERTEX, m_GlobalConstant);
		SetConstantBuffer(0, SHADER_TYPE_PIXEL, m_GlobalConstant);
		SetConstantBuffer(9, SHADER_TYPE_VERTEX, m_CameraConstant);
		SetConstantBuffer(9, SHADER_TYPE_PIXEL, m_CameraConstant);

		{
			ProfileBeginEvent(0, "Build Acceleration Structure");

			ProfileEndEvent();
		}

		{
			ProfileBeginEvent(0, "Forward Pass");

			RasterizerDesc rs;
			rs.desc.AntialiasedLineEnable = FALSE;
			rs.desc.CullMode = D3D12_CULL_MODE_NONE;
			rs.desc.DepthBias = 0.f;
			rs.desc.DepthBiasClamp = 0.f;
			rs.desc.DepthClipEnable = TRUE;
			rs.desc.FillMode = D3D12_FILL_MODE_SOLID;
			rs.desc.FrontCounterClockwise = FALSE;
			rs.desc.MultisampleEnable = FALSE;
			//rs.desc.ScissorEnable = FALSE;
			rs.desc.SlopeScaledDepthBias = 0.f;
			SetRasterizerState(rs);

			for (int i = 0; i < gData.m_GraphObjectDA.GetSize(); i++)
			{
				GraphObject * it = gData.m_GraphObjectDA[i];
				it->Draw();
			}

			ProfileEndEvent();
		}

		{
			ProfileBeginEvent(0, "Post Process");
			GetHAL().GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(hdrtex->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

			{
				SetShaderResource(0, SHADER_TYPE_COMPUTE, m_HdrRenderTarget);
				SetUAV(0, m_RenderTarget);
				BindComputePipelineState(ShaderMap::ComputeHistogramCS);
				Dispatch(1, 1, 1);
			}

			D3D12_CPU_DESCRIPTOR_HANDLE rtv = GetHAL().GetCurrentBackBufferView();
			GetHAL().GetCommandList()->OMSetRenderTargets(1, &rtv, false, &GetHAL().GetDSV());

			SetShaderResource(0, SHADER_TYPE_PIXEL, m_HdrRenderTarget);
			BindGraphicPipelineState(ShaderMap::ScreenVertexVS, ShaderMap::TonemappingPS);
			FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

			ProfileEndEvent();
		}

		DrawImGUI();

		GetHAL().PresentFrame();

		ProfileEndEvent();
		m_FrameIndex++;
	}

	void DXRenderer::SetShaderResource(U32 Slot, EShaderType Type, Bitmap* Texture)
	{
		TextureLink * tex = reinterpret_cast<TextureLink*>(Texture->GetBinHwResId());
		if ((U64)tex != BM_INVALIDHWRESID)
		{
			GetHAL().SetShaderResource(Slot, Type, tex);
		}
	}

	void DXRenderer::SetUAV(U32 Slot, Bitmap* Texture)
	{
		TextureLink* tex = reinterpret_cast<TextureLink*>(Texture->GetBinHwResId());
		if ((U64)tex != BM_INVALIDHWRESID)
		{
			GetHAL().SetUAV(Slot, tex);
		}
	}

	void DXRenderer::InitSurface()
	{
	}

	void DXRenderer::InitShaders()
	{
		GetHAL().InitShaders();

		// Register common shaders
		Renderer::InitShaders();
	}

	void DXRenderer::ReleaseAllResources()
	{
	}

	void DXRenderer::FullScreenQuad(Vec2f scale,Vec2f offset)
	{
		PushVertexDeclaration(m_ScreenVertexDecl);
		PushStreamSource(0,m_FullscreenQuadVB,0,32);

		/*
		XMVECTOR * _vec = (XMVECTOR*)(m_VSConstantCache);
		_vec[USER_CST+0] = XMVectorSet(scale.x,scale.y,offset.x,offset.y);
		UpdateVSConstants();
		*/

		SetPrimitiveTopology(PRIM_TRIANGLESTRIP);
		DrawInstanced(4,1,0,0);
	}

	void DXRenderer::PostProcess()
	{
	}

	void DXRenderer::PushVertexDeclaration(VertexDeclaration* Decl)
	{
		GetHAL().SetVertexDeclaration(Decl);
	}

	void DXRenderer::PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride)
	{
		GetHAL().SetStreamSource(StreamNumber, Buffer, Offset, Stride);
		//m_StateCache.VB = dxbuffer;
	}

	void DXRenderer::PushIndices(IndexBuffer* Buffer,U32 _Fmt)
	{
		if(Buffer)
		{
			GetHAL().SetIndices(Buffer, _Fmt);
			//m_StateCache.IB = Buffer;
		}
	}

	void DXRenderer::SetPrimitiveTopology(PrimitiveType Topology)
	{
		GetHAL().SetPrimitiveTopology(Topology);
	}

	void DXRenderer::PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount)
	{
		MESSAGE("DXRenderer::PushDrawIndexed Fait pas grand chose");
	}

	void DXRenderer::BindGraphicPipelineState(ShaderKernel* VS, ShaderKernel* PS)
	{
		GetHAL().BindGraphicPipelineState(VS, PS);
	}
	void DXRenderer::BindComputePipelineState(ShaderKernel* CS)
	{
		GetHAL().BindComputePipelineState(CS);
	}

	void DXRenderer::PushMaterial(Material* Mat)
	{
		Bitmap* bm = NULL;
		bm = Mat->GetBitmap(MTL_STAGE_ALBEDO);
		if(bm)
		{
			SetShaderResource(0, SHADER_TYPE_PIXEL, bm);
		}
		bm = Mat->GetBitmap(MTL_STAGE_NORMAL);
		if(bm)
		{
			SetShaderResource(1, SHADER_TYPE_PIXEL, bm);
		}
		bm = Mat->GetBitmap(MTL_STAGE_ROUGHNESS);
		if(bm)
		{
			SetShaderResource(2, SHADER_TYPE_PIXEL, bm);
		}
	}

};

#endif //_PC

