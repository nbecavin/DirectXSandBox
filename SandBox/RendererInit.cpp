#include <Renderer.h>
#include <D3D12HAL.h>
#include <RenderGraph.h>
#include <Bitmap.h>

namespace sys {

	int Renderer::Init()
	{
		m_GlobalParams = {};
		m_GlobalParams.Visualize = EVIZ_LIT;

		SizeX = 1920;
		SizeY = 1080;

		m_HAL = std::make_unique<D3D12HAL>();
		m_HAL->Init(SizeX, SizeY, this);

		InitShaders();
		InitStaticDatas();
		InitImGUI();

		m_RenderGraph = new RenderGraph();
		return 0;
	}

	void Renderer::Shut()
	{

	}

	bool Renderer::InitStaticDatas()
	{
		// Render targets
		m_RenderTarget = new Bitmap();
		m_RenderTarget->SetSize(SizeX,SizeY);
		m_RenderTarget->SetFormat(BM_R8G8B8A8_UNORM);
		m_RenderTarget->SetType(BM_TYPE_2D);
		m_RenderTarget->DisableFlags(BM_SRGB);
		m_RenderTarget->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV | BM_USAGE_UAV);
		GetHAL()->CreateTexture(m_RenderTarget);

		m_HdrRenderTarget = new Bitmap();
		m_HdrRenderTarget->SetSize(SizeX,SizeY);
		m_HdrRenderTarget->SetFormat(BM_R16G16B16A16_FLOAT);
		m_HdrRenderTarget->SetType(BM_TYPE_2D);
		m_HdrRenderTarget->DisableFlags(BM_SRGB);
		m_HdrRenderTarget->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV | BM_USAGE_UAV);
		GetHAL()->CreateTexture(m_HdrRenderTarget);

		m_gBuffer[0] = new Bitmap();
		m_gBuffer[0]->SetSize(SizeX,SizeY);
		m_gBuffer[0]->SetFormat(BM_R8G8B8A8_UNORM);
		m_gBuffer[0]->SetType(BM_TYPE_2D);
		m_gBuffer[0]->DisableFlags(BM_SRGB);
		m_gBuffer[0]->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV | BM_USAGE_UAV);
		GetHAL()->CreateTexture(m_gBuffer[0]);

		m_lightBuffer = new Bitmap();
		m_lightBuffer->SetSize(SizeX,SizeY);
		m_lightBuffer->SetFormat(BM_R8G8B8A8_UNORM);
		m_lightBuffer->SetType(BM_TYPE_2D);
		m_lightBuffer->DisableFlags(BM_SRGB);
		m_lightBuffer->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV | BM_USAGE_UAV);
		GetHAL()->CreateTexture(m_lightBuffer);

		m_linearZBuffer = new Bitmap();
		m_linearZBuffer->SetSize(SizeX,SizeY);
		m_linearZBuffer->SetFormat(BM_R32_FLOAT);
		m_linearZBuffer->SetType(BM_TYPE_2D);
		m_linearZBuffer->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV | BM_USAGE_UAV);
		GetHAL()->CreateTexture(m_linearZBuffer);

		m_ssaoBuffer = new Bitmap();
		m_ssaoBuffer->SetSize(SizeX,SizeY);
		m_ssaoBuffer->SetFormat(BM_R8G8B8A8_UNORM);
		m_ssaoBuffer->SetType(BM_TYPE_2D);
		m_ssaoBuffer->DisableFlags(BM_SRGB);
		m_ssaoBuffer->SetUsage(BM_USAGE_SRV | BM_USAGE_RTV);
		GetHAL()->CreateTexture(m_ssaoBuffer);

		m_CameraConstant = GetHAL()->CreateConstantBuffer(sizeof(CameraConstant));
		m_GlobalConstant = GetHAL()->CreateConstantBuffer(sizeof(GlobalParameters));

		return TRUE;
	}

};
