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
		XMVECTOR det;
		camCst->InvProjMatrix = XMMatrixTranspose(XMMatrixInverse(&det, proj));
		m_CameraConstant->Unlock();

		// No geometry shader

		//GetCommandList()->PSSetSamplers(0, 1, &m_DefaultSS);
		//GetCommandList()->PSSetSamplers(1, 1, &m_DefaultSS);
		//GetCommandList()->PSSetSamplers(2, 1, &m_DefaultSS);

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

#if 0
//		D3DPERF_BeginEvent(0,L"PostProcess");

		//TextureLink * tex = reinterpret_cast<TextureLink*>(m_lightBuffer->GetBinHwResId());
		TextureLink * hdrtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
		TextureLink * rtex = reinterpret_cast<TextureLink*>(m_RenderTarget->GetBinHwResId());

		GetCommandList()->OMSetRenderTargets(1, &rtex->Surface, m_DepthBuffer);
		GetCommandList()->PSSetShaderResources(0, 1, &hdrtex->ShaderView);
		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_COLORGRADING);
		FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

		// Setup the viewport
		D3D12_VIEWPORT vp;
		vp.Width = (FLOAT)SizeX;
		vp.Height = (FLOAT)SizeY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		GetCommandList()->RSSetViewports(1, &vp);
		GetCommandList()->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);
		GetCommandList()->PSSetShaderResources(0, 1, &rtex->ShaderView);

		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_FXAA);

//		FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

		//		D3DPERF_EndEvent();

				// Draw debug stuff
		{
			/*
			TextureLink * tex = reinterpret_cast<TextureLink*>(m_gBuffer[0]->GetBinHwResId());
			DeviceContext->PSSetShaderResources(0,1,&tex->ShaderView);
			PushShader(SHADER_VS_BASE_SCREENVERTEX);
			PushShader(SHADER_PS_POSTPROC_PASSTHROUGH);
			FullScreenQuad(Vec2f(0.33f,0.33f),Vec2f(-0.65f,+0.65f));

			tex = reinterpret_cast<TextureLink*>(m_lightBuffer->GetBinHwResId());
			DeviceContext->PSSetShaderResources(0,1,&tex->ShaderView);
			PushShader(SHADER_VS_BASE_SCREENVERTEX);
			PushShader(SHADER_PS_POSTPROC_PASSTHROUGH);
			FullScreenQuad(Vec2f(0.33f,0.33f),Vec2f(-0.65f,-0.05f));

			tex = reinterpret_cast<TextureLink*>(m_ssaoBuffer->GetBinHwResId());
			DeviceContext->PSSetShaderResources(0,1,&tex->ShaderView);
			PushShader(SHADER_VS_BASE_SCREENVERTEX);
			PushShader(SHADER_PS_POSTPROC_PASSTHROUGH);
			FullScreenQuad(Vec2f(0.33f,0.33f),Vec2f(0.65f,+0.65f));
			*/
		}

		ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
		GetCommandList()->PSSetShaderResources(0, 1, ppSRVNULL);
#endif

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
		MESSAGE("DXRenderer::FullScreenQuad Fait pas grand chose");
#if 0
		PushVertexDeclaration(m_ScreenVertexDecl);
		PushStreamSource(0,m_FullscreenQuadVB,0,32);

		XMVECTOR * _vec = (XMVECTOR*)(m_VSConstantCache);
		_vec[USER_CST+0] = XMVectorSet(scale.x,scale.y,offset.x,offset.y);

		UpdateVSConstants();

#ifdef _PCDX12
		ID3D12PipelineState* PSO;
		GetDevice()->CreateGraphicsPipelineState(&GetHAL().GetPipelineState(), IID_PPV_ARGS(&PSO));

		PSO->SetName(L"test");

		GetCommandList()->SetPipelineState(PSO);
#endif

		GetCommandList()->IASetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetCommandList()->DrawInstanced(4,1,0,0);

#ifdef _PCDX12
		PSO->Release();
#endif
#endif
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

	void DXRenderer::PushShader(U32 _ShaderUID)
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));
		GetHAL().SetShaders(Type, SID);
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

