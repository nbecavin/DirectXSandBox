#include <Renderer.h>
#include <ShaderConstants.h>
#include <RenderGraph.h>
#include <D3D12HAL.h>

// RTXDI
// https://github.com/NVIDIA-RTX/RTXDI/blob/main/Doc/Integration.md

#include <Rtxdi/RtxdiParameters.h>
#include <Rtxdi/DI/ReSTIRDI.h>
#include <Rtxdi/GI/ReSTIRGI.h>

namespace sys {

	rtxdi::ReSTIRDIContext* m_RtxDIcontext = nullptr;

	void Renderer::BeginFrame(float dTime)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = dTime;              // set the time elapsed since the previous frame (in seconds)
		io.DisplaySize.x = SizeX;             // set the current display width
		io.DisplaySize.y = SizeY;             // set the current display height here
		ImGui::NewFrame();
	}

	void Renderer::MainLoop()
	{
		auto* hal = GetHAL();
		auto* d3d = GetHALPtr<D3D12HAL>();

		m_Camera->Update();

		hal->ProfileBeginEventArgs(0, "Frame %d", hal->GetFrameIndex());

		if (!m_RtxDIcontext)
		{
			rtxdi::ReSTIRDIStaticParameters params = {};
			params.RenderWidth = SizeX;
			params.RenderHeight = SizeY;
			m_RtxDIcontext = new rtxdi::ReSTIRDIContext(params);
		}

		ImGui::Begin("Stats");
		ImGui::Text("FPS %f", 1.f / GetDeltaTime());
		ImGui::Text("Time %fms", GetDeltaTime() * 1000.f);
		ImGui::End();

		HRESULT hr;

		d3d->SetAndClearRenderTarget();

		TextureLink* hdrtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
		d3d->GetCommandList()->OMSetRenderTargets(1, &hdrtex->m_RTV, true, &d3d->GetDSV());
		d3d->GetCommandList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(hdrtex->Resource12, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

		// Setup the viewport
		D3D12_VIEWPORT vp;
		vp.Width = (FLOAT)SizeX;
		vp.Height = (FLOAT)SizeY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		d3d->SetViewports(vp);

		DepthStencilDesc ds;
		d3d->SetDepthStencilState(ds);
		RasterizerDesc rs;
		d3d->SetRasterizerState(rs);

		//une view et une proj de base
		XMMATRIX m, proj, view;
		XMVECTOR eye = XMVectorSet(0, 1, -5, 0);
		XMVECTOR at = XMVectorSet(0, 1, 0, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);
		view = XMMatrixLookAtRH(eye, at, up);

		view = *reinterpret_cast<XMMATRIX*>(&m_Camera->GetViewMatrix());

		proj = XMMatrixPerspectiveRH(.25, .25f / ((float)SizeX / (float)SizeY), 0.1, 1000);

		CameraConstant* camCst;
		m_CameraConstant->Lock(0, 0, (void**)&camCst);
		camCst->ProjMatrix = XMMatrixTranspose(proj);
		camCst->ViewMatrix = XMMatrixTranspose(view);
		camCst->EyeWorld = m_Camera->GetWorldPosition();
		XMVECTOR det;
		camCst->InvProjMatrix = XMMatrixTranspose(XMMatrixInverse(&det, proj));
		camCst->InvViewMatrix = XMMatrixTranspose(XMMatrixInverse(&det, view));
		m_CameraConstant->Unlock();

		GlobalParameters* globalParam;
		m_GlobalConstant->Lock(0, 0, (void**)&globalParam);
		globalParam[0] = m_GlobalParams;
		m_GlobalConstant->Unlock();

		hal->SetConstantBuffer(0, SHADER_TYPE_VERTEX, m_GlobalConstant);
		hal->SetConstantBuffer(0, SHADER_TYPE_PIXEL, m_GlobalConstant);
		hal->SetConstantBuffer(0, SHADER_TYPE_COMPUTE, m_GlobalConstant);
		hal->SetConstantBuffer(9, SHADER_TYPE_VERTEX, m_CameraConstant);
		hal->SetConstantBuffer(9, SHADER_TYPE_PIXEL, m_CameraConstant);
		hal->SetConstantBuffer(9, SHADER_TYPE_COMPUTE, m_CameraConstant);

		gData.Rdr->GetRenderGraph()->DrawFrame();

		DrawImGUI();

		d3d->PresentFrame();

		hal->ProfileEndEvent();
		hal->AdvanceFrameIndex();

	}

};
