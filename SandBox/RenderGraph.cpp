#include <RenderGraph.h>
#include <RenderHAL.h>
#include <Renderer.h>
#include <imgui.h>
#include <D3D12HAL.h>
#include <RenderHAL.h>

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
	TextureLink* hdrTextureLink = reinterpret_cast<TextureLink*>(hdrRenderTarget->GetBinHwResId());

	U32 sceneX = gData.Rdr->GetSizeX();
	U32 sceneY = gData.Rdr->GetSizeY();

	{
		hal->ProfileBeginEvent(0, "Build Acceleration Structure");

		gData.Scene->BuilRaytracing();

		hal->ProfileEndEvent();
	}

	{
		hal->ProfileBeginEvent(0, "Forward Pass");

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

		gData.Scene->DrawScene();

		hal->ProfileEndEvent();
	}

	if (p.Visualize == EVIZ_PATHTRACER)
	{
		hal->ProfileBeginEvent(0, "Pathtracer");

		hal->ProfileEndEvent();
	}

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
}
