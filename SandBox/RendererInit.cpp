#include <Renderer.h>
#include <Bitmap.h>

namespace sys {

	struct SCREEN_VERTEX
	{
		Vec4f	pos;
		Vec2f	tex;
	};

	bool Renderer::InitStaticDatas()
	{
		// Fullscreen quad for post processing
		{
			// Create a screen quad for render to texture operations
			SCREEN_VERTEX svQuad[4];
			svQuad[0].pos.Set( -1.0f, 1.0f, 0.5f, 1.0f );
			svQuad[0].tex.Set( 0.0f, 0.0f );
			svQuad[1].pos.Set( 1.0f, 1.0f, 0.5f, 1.0f );
			svQuad[1].tex.Set( 1.0f, 0.0f );
			svQuad[2].pos.Set( -1.0f, -1.0f, 0.5f, 1.0f );
			svQuad[2].tex.Set( 0.0f, 1.0f );
			svQuad[3].pos.Set( 1.0f, -1.0f, 0.5f, 1.0f );
			svQuad[3].tex.Set( 1.0f, 1.0f );

			// Generate runtime buffers
			m_FullscreenQuadVB = CreateVertexBuffer(sizeof(svQuad),0,NULL);
			SCREEN_VERTEX * pVtxData;
			if(m_FullscreenQuadVB && m_FullscreenQuadVB->Lock(0,0,(void**)&pVtxData)==true)
			{
				memcpy(pVtxData,svQuad,sizeof(svQuad));
				m_FullscreenQuadVB->Unlock();
			}

			VertexElement DeclDesc[] = 
			{
				{ 0, 0, DECL_FMT_FLOAT4, DECL_POSITION },
				{ 0, 16, DECL_FMT_FLOAT2, DECL_TEXCOORD0 },
				DECL_END()
			};

			m_ScreenVertexDecl = gData.Rdr->CreateVertexDecl( DeclDesc, SHADER_VS_BASE_SCREENVERTEX );
		}

		// Render targets
		m_RenderTarget = new Bitmap();
		m_RenderTarget->SetSize(SizeX,SizeY);
		m_RenderTarget->SetFormat(BM_R8G8B8A8_UNORM);
		m_RenderTarget->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_RenderTarget);

		m_HdrRenderTarget = new Bitmap();
		m_HdrRenderTarget->SetSize(SizeX,SizeY);
		m_HdrRenderTarget->SetFormat(BM_R8G8B8A8_UNORM);
		m_HdrRenderTarget->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_HdrRenderTarget);

		m_gBuffer[0] = new Bitmap();
		m_gBuffer[0]->SetSize(SizeX,SizeY);
		m_gBuffer[0]->SetFormat(BM_R8G8B8A8_UNORM);
		m_gBuffer[0]->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_gBuffer[0]);

		m_lightBuffer = new Bitmap();
		m_lightBuffer->SetSize(SizeX,SizeY);
		m_lightBuffer->SetFormat(BM_R8G8B8A8_UNORM);
		m_lightBuffer->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_lightBuffer);

		m_linearZBuffer = new Bitmap();
		m_linearZBuffer->SetSize(SizeX,SizeY);
		m_linearZBuffer->SetFormat(BM_R32_FLOAT);
		m_linearZBuffer->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_linearZBuffer);

		m_ssaoBuffer = new Bitmap();
		m_ssaoBuffer->SetSize(SizeX,SizeY);
		m_ssaoBuffer->SetFormat(BM_R8G8B8A8_UNORM);
		m_ssaoBuffer->SetType(BM_TYPE_2D|BM_TYPE_RT);
		CreateTexture(m_ssaoBuffer);

		return TRUE;
	}

};
