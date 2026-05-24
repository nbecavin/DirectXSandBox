#include <RenderGraph.h>
#include <RenderHAL.h>
#include <Renderer.h>
#include <imgui.h>
#include <D3D12HAL.h>
#include <RenderHAL.h>
#include <GpuScene.h>

RenderGraph::RenderGraph()
{
}

RenderGraph::~RenderGraph()
{
}

void RenderGraph::DrawFrame()
{
	GlobalParameters p = gData.Rdr->GetGlobalParameters();

	gData.Scene->GatherDrawObjects();

	sys::RenderHAL* hal = gData.Rdr->GetHAL();
	auto* d3d = hal->AsPtr<D3D12HAL>();

	Bitmap* renderTarget = gData.Rdr->m_RenderTarget;
	Bitmap* hdrRenderTarget = gData.Rdr->m_HdrRenderTarget;
	Bitmap* shadowBuffer = gData.Rdr->m_shadowBuffer;
	TextureLink* hdrTextureLink = reinterpret_cast<TextureLink*>(hdrRenderTarget->GetBinHwResId());
	TextureLink* shadowTextureLink = reinterpret_cast<TextureLink*>(shadowBuffer->GetBinHwResId());

	U32 sceneX = gData.Rdr->GetSizeX();
	U32 sceneY = gData.Rdr->GetSizeY();

	// Build Acceleration Structure
	{
		hal->ProfileBeginEvent(0, "Build Acceleration Structure");

		gData.Scene->BuilRaytracing();

		hal->ProfileEndEvent();
	}

	// Populate gbuffer
	{
		hal->ProfileBeginEvent(0, "GBuffer Pass");

		TextureLink* gbuffer0 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[0]->GetBinHwResId());
		TextureLink* gbuffer1 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[1]->GetBinHwResId());
		TextureLink* gbuffer2 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[2]->GetBinHwResId());
		TextureLink* gbuffer3 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[3]->GetBinHwResId());
		TextureLink* linear_depth = reinterpret_cast<TextureLink*>(gData.Rdr->m_linearZBuffer->GetBinHwResId());

		TextureLink* rtvs[] = {
			gbuffer0,
			gbuffer1,
			gbuffer2,
			gbuffer3
		};

		hal->SetRenderTargets(4, rtvs);

		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer0->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer1->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer2->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer3->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

		gData.Scene->DrawScene(sys::GpuScene::PassHint::GBuffer);

		// Copy depth buffer
		{
			d3d->GetCommandList()->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(d3d->GetDepthStencil(),
					D3D12_RESOURCE_STATE_DEPTH_WRITE,
					D3D12_RESOURCE_STATE_COPY_SOURCE)
			);
			d3d->GetCommandList()->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(linear_depth->Resource12,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
					D3D12_RESOURCE_STATE_COPY_DEST)
			);

			d3d->GetCommandList()->CopyResource(
				linear_depth->Resource12,
				d3d->GetDepthStencil()
			);

			d3d->GetCommandList()->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(d3d->GetDepthStencil(),
					D3D12_RESOURCE_STATE_COPY_SOURCE,
					D3D12_RESOURCE_STATE_DEPTH_WRITE)
			);
			d3d->GetCommandList()->ResourceBarrier(1,
				&CD3DX12_RESOURCE_BARRIER::Transition(linear_depth->Resource12,
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
			);
		}

		hal->ProfileEndEvent();
	}

	// Raytraced sun shadow
	{
		hal->ProfileBeginEvent(0, "Raytraced Sun Shadows");

		U32 threadGroupX = (sceneX + 7) / 8;
		U32 threadGroupY = (sceneY + 7) / 8;

		TextureLink* gbuffer0 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[0]->GetBinHwResId());
		TextureLink* gbuffer1 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[1]->GetBinHwResId());
		TextureLink* gbuffer2 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[2]->GetBinHwResId());
		TextureLink* gbuffer3 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[3]->GetBinHwResId());

		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer0->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer1->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer2->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer3->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		hal->SetShaderResource(4, SHADER_TYPE_COMPUTE, gData.Rdr->m_gBuffer[0]);
		hal->SetShaderResource(5, SHADER_TYPE_COMPUTE, gData.Rdr->m_gBuffer[1]);
		hal->SetShaderResource(6, SHADER_TYPE_COMPUTE, gData.Rdr->m_gBuffer[2]);
		hal->SetShaderResource(7, SHADER_TYPE_COMPUTE, gData.Rdr->m_gBuffer[3]);
		hal->SetShaderResource(8, SHADER_TYPE_COMPUTE, gData.Rdr->m_linearZBuffer);

		d3d->GetCommandList()->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(shadowTextureLink->Resource12,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		);

		hal->SetAccelerationStructure(0, SHADER_TYPE_COMPUTE, gData.Scene->GetRaytracingScene()->m_TLAS);
		hal->SetUAV(0, shadowBuffer);
		hal->BindComputePipelineState(ShaderMap::RaytracingSunShadowsInlineRGS);
		hal->Dispatch(threadGroupX, threadGroupY, 1);

		d3d->GetCommandList()->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(shadowTextureLink->Resource12,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		);

		hal->ProfileEndEvent();
	}

	// Forward Pass
	{
		TextureLink* hdrtex = reinterpret_cast<TextureLink*>(hdrRenderTarget->GetBinHwResId());
		hal->SetRenderTargets(1, &hdrtex);

		hal->ProfileBeginEvent(0, "Forward Pass");

		hal->SetShaderResource(0, SHADER_TYPE_PIXEL, shadowBuffer);

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
		hal->SetRasterizerState(rs);

		DepthStencilDesc ds;
		ds.desc.DepthEnable = TRUE;
		ds.desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		ds.desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		ds.desc.StencilEnable = FALSE;
		ds.desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		ds.desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		ds.desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		ds.desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		ds.desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		hal->SetDepthStencilState(ds);

		gData.Scene->DrawScene(sys::GpuScene::PassHint::Forward);

		hal->ProfileEndEvent();
	}

	// Pathtracer
	if (p.Visualize == EVIZ_PATHTRACER)
	{
		hal->ProfileBeginEvent(0, "Pathtracer");

		hal->ProfileEndEvent();
	}

	// Raytracing debug
	if(p.Visualize == EVIZ_SHOW_RAYTRACING_DEBUG)
	{
		hal->ProfileBeginEvent(0, "Raytracing debug");
		d3d->GetCommandList()->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(hdrTextureLink->Resource12,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS)
		);

		U32 threadGroupX = (sceneX + 7) / 8;
		U32 threadGroupY = (sceneY + 7) / 8;

		hal->SetAccelerationStructure(0, SHADER_TYPE_COMPUTE, gData.Scene->GetRaytracingScene()->m_TLAS);
		hal->SetUAV(0, hdrRenderTarget);
		hal->BindComputePipelineState(ShaderMap::RaytracingDebugInlineRGS);
		hal->Dispatch(threadGroupX, threadGroupY, 1);

		d3d->GetCommandList()->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(hdrTextureLink->Resource12,
				D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
				D3D12_RESOURCE_STATE_RENDER_TARGET)
		);

		hal->ProfileEndEvent();
	}

	// Post Process
	{
		hal->ProfileBeginEvent(0, "Post Process");

		d3d->GetCommandList()->ResourceBarrier(1,
			&CD3DX12_RESOURCE_BARRIER::Transition(hdrTextureLink->Resource12,
				D3D12_RESOURCE_STATE_RENDER_TARGET,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
		);

		//{
		//	hal->SetShaderResource(0, SHADER_TYPE_COMPUTE, hdrRenderTarget);
		//	hal->SetUAV(0, renderTarget);
		//	hal->BindComputePipelineState(ShaderMap::ComputeHistogramCS);
		//	hal->Dispatch(1, 1, 1);
		//}

		D3D12_CPU_DESCRIPTOR_HANDLE rtv = d3d->GetCurrentBackBufferView();
		d3d->GetCommandList()->OMSetRenderTargets(1, &rtv, false, &d3d->GetDSV());

		hal->SetShaderResource(0, SHADER_TYPE_PIXEL, hdrRenderTarget);
		hal->BindGraphicPipelineState(ShaderMap::ScreenVertexVS, ShaderMap::TonemappingPS);
		hal->FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

		hal->ProfileEndEvent();
	}

	// Debug Gbuffer
	bool bDebugGbuffer = p.Visualize == EVIZ_SHOW_ALBEDO || p.Visualize == EVIZ_SHOW_NORMAL || p.Visualize == EVIZ_SHOW_ROUGHNESS || p.Visualize == EVIZ_SHOW_METAL || p.Visualize == EVIZ_SHOW_DEPTH;
	if(bDebugGbuffer)
	{
		hal->ProfileBeginEvent(0, "Debug Gbuffer");

		TextureLink* gbuffer0 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[0]->GetBinHwResId());
		TextureLink* gbuffer1 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[1]->GetBinHwResId());
		TextureLink* gbuffer2 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[2]->GetBinHwResId());
		TextureLink* gbuffer3 = reinterpret_cast<TextureLink*>(gData.Rdr->m_gBuffer[3]->GetBinHwResId());

		//d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer0->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		//d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer1->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		//d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer2->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));
		//d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer3->Resource12, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE));

		hal->SetShaderResource(4, SHADER_TYPE_PIXEL, gData.Rdr->m_gBuffer[0]);
		hal->SetShaderResource(5, SHADER_TYPE_PIXEL, gData.Rdr->m_gBuffer[1]);
		hal->SetShaderResource(6, SHADER_TYPE_PIXEL, gData.Rdr->m_gBuffer[2]);
		hal->SetShaderResource(7, SHADER_TYPE_PIXEL, gData.Rdr->m_gBuffer[3]);
		hal->SetShaderResource(8, SHADER_TYPE_PIXEL, gData.Rdr->m_linearZBuffer);

		D3D12_CPU_DESCRIPTOR_HANDLE rtv = d3d->GetCurrentBackBufferView();
		d3d->GetCommandList()->OMSetRenderTargets(1, &rtv, false, &d3d->GetDSV());

		hal->SetShaderResource(0, SHADER_TYPE_PIXEL, hdrRenderTarget);
		hal->BindGraphicPipelineState(ShaderMap::ScreenVertexVS, ShaderMap::DebugGbufferPS);
		hal->FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer0->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer1->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer2->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(gbuffer3->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

		hal->ProfileEndEvent();
	}
}
