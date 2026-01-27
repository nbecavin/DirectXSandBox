#include <RenderGraph.h>
#include <RenderHAL.h>
#include <Renderer.h>
#include <imgui.h>

RenderGraph::RenderGraph()
{
}

RenderGraph::~RenderGraph()
{
}

void RenderGraph::DrawFrame()
{
	sys::RenderHAL* device = gData.Rdr->GetHAL();

	{
		device->ProfileBeginEvent(0, "Build Acceleration Structure");

		device->BuildTLAS();

		device->ProfileEndEvent();
	}

#if 0

	{
		gData.Rdr->ProfileBeginEvent(0, "Forward Pass");

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
		gData.Rdr->SetRasterizerState(rs);

		for (int i = 0; i < gData.m_GraphObjectDA.GetSize(); i++)
		{
			GraphObject* it = gData.m_GraphObjectDA[i];
			it->Draw();
		}

		gData.Rdr->ProfileEndEvent();
	}

	{
		gData.Rdr->ProfileBeginEvent(0, "Post Process");
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
		gData.Rdr->FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

		gData.Rdr->ProfileEndEvent();
	}
#endif
}
