#if defined(_PC)

#include <DXRenderer.h>
#include <Material.h>
#include <Bitmap.h>
#include "SandBox.h"
#include <WinMain.h>
#include <..\..\Shaders\ShaderRegs.h>
#include <ShaderConstants.h>

namespace sys {

	Vec4f sys::m_VSConstantCache[VS_CONSTANT_MAX_COUNT];

	void InitDevice();
	void ResetDevice();

	void DXRenderer::MainLoop()
	{
		m_Camera->Update();

		HRESULT hr;

#if defined(_PCDX12)
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
		GetCommandList()->ClearRenderTargetView(GetHAL().GetCurrentBackBufferView(), ClearColor, 0, nullptr);
#elif defined(_PCDX11)
		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
		GetCommandList()->ClearRenderTargetView(m_BackBuffer, ClearColor);
		GetCommandList()->ClearDepthStencilView(m_DepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
#endif



		//une view et une proj de base
		XMMATRIX m, proj, view;
		XMVECTOR eye = XMVectorSet(0, 1, -5, 0);
		XMVECTOR at = XMVectorSet(0, 1, 0, 0);
		XMVECTOR up = XMVectorSet(0, 1, 0, 0);
		view = XMMatrixLookAtRH(eye, at, up);

		view = *reinterpret_cast<XMMATRIX*>(&m_Camera->GetViewMatrix());

		proj = XMMatrixPerspectiveRH(.25, .25f / ((float)SizeX / (float)SizeY), 0.1, 1000);

		CameraConstant camCst;
		camCst.ProjMatrix = XMMatrixTranspose(proj);
		camCst.ViewMatrix = XMMatrixTranspose(view);
		XMVECTOR det;
		camCst.InvProjMatrix = XMMatrixTranspose(XMMatrixInverse(&det, proj));
		//camCst.InvViewProjMatrix = ;

#if defined(_PCDX11_)
		UpdateConstantBuffer(m_CameraConstant, &camCst, sizeof(camCst));
#endif

		XMMATRIX * _mat = (XMMATRIX*)(m_VSConstantCache);
		_mat[0] = XMMatrixTranspose(proj);
		_mat[1] = XMMatrixTranspose(view);
		Vec4f * _vec = (Vec4f*)(m_VSConstantCache);
		_vec[EYE_CST] = m_Camera->GetWorldPosition();

#if defined(_PCDX11_)
		// No geometry shader
		GetCommandList()->GSSetShader(NULL, NULL, 0);

		GetCommandList()->RSSetState(m_DefaultRS);
		GetCommandList()->OMSetDepthStencilState(m_DefaultDS, 0);
		GetCommandList()->PSSetSamplers(0, 1, &m_DefaultSS);
		GetCommandList()->PSSetSamplers(1, 1, &m_DefaultSS);
		GetCommandList()->PSSetSamplers(2, 1, &m_DefaultSS);

		GetCommandList()->VSSetConstantBuffers(8, 1, &m_SHHemisphere);
		GetCommandList()->PSSetConstantBuffers(8, 1, &m_SHHemisphere);

		GetCommandList()->VSSetConstantBuffers(9, 1, &m_CameraConstant);
		GetCommandList()->PSSetConstantBuffers(9, 1, &m_CameraConstant);
#endif

		/*
				//
		//		D3DPERF_BeginEvent(0,L"GBuffer");
				{
					TextureLink * rtex = reinterpret_cast<TextureLink*>( m_gBuffer[0]->GetBinHwResId());
					DeviceContext->OMSetRenderTargets(1,&rtex->Surface,m_DepthBuffer);
					float ClearColor2[4] = { 1.0f, 1.f, 1.f, 1.0f }; // red,green,blue,alpha
					DeviceContext->ClearRenderTargetView( rtex->Surface, ClearColor2 );

					// Setup the viewport
					D3D11_VIEWPORT vp;
					vp.Width = (FLOAT)SizeX;
					vp.Height = (FLOAT)SizeY;
					vp.MinDepth = 0.0f;
					vp.MaxDepth = 1.0f;
					vp.TopLeftX = 0;
					vp.TopLeftY = 0;
					DeviceContext->RSSetViewports( 1, &vp );

					for(int i=0;i<gData.m_GraphObjectDA.GetSize();i++)
					{
						GraphObject * it = gData.m_GraphObjectDA[i];
						it->DrawGBuffer();
					}
				}
		//		D3DPERF_EndEvent();

		//		D3DPERF_BeginEvent(0xff0000ff,L"Deferred");
				{
					TextureLink * rtex = reinterpret_cast<TextureLink*>(m_lightBuffer->GetBinHwResId());
					DeviceContext->OMSetRenderTargets(1,&rtex->Surface,m_DepthBuffer);
					float ClearColor2[4] = { 0.0f, 0.f, 0.f, 1.0f }; // red,green,blue,alpha
					DeviceContext->ClearRenderTargetView( rtex->Surface, ClearColor2 );
					DeviceContext->OMSetDepthStencilState(m_DSS_NoZWrite,0);
					PushShader(SHADER_VS_BASE_SCREENVERTEX);
					PushShader(SHADER_PS_POSTPROC_PASSTHROUGH);
					FullScreenQuad(Vec2f(1.f,1.f),Vec2f(0.f,0.f));
					DeviceContext->OMSetDepthStencilState(m_DefaultDS,0);

					// SSAO
		//			D3DPERF_BeginEvent(0,L"SSAO");
					{
						DeviceContext->OMSetDepthStencilState(m_DSS_NoZWrite,0);

						PostProcessConstant m_ShaderConstant;

						m_ShaderConstant.gPostProcess[0].x = 1.f/(float)SizeX;
						m_ShaderConstant.gPostProcess[0].y = 1.f/(float)SizeY;
						m_ShaderConstant.gPostProcess[1].w = (float)SizeX/(float)SizeY;
						m_ShaderConstant.gPostProcess[1].w = 1.f;
						m_ShaderConstant.gPostProcess[2].x = 1.f;
						m_ShaderConstant.gPostProcess[2].y = 1.f;
						m_ShaderConstant.gPostProcess[3].x = 0.03f;
						m_ShaderConstant.gPostProcess[3].y = 0.09f;
						m_ShaderConstant.gPostProcess[3].z = 0.15f;
						m_ShaderConstant.gPostProcess[3].w = 0.45f;

						DeviceContext->PSSetSamplers(0,1,&m_NoBilinearSS);

						TextureLink * linearDepthBuffer = reinterpret_cast<TextureLink*>(m_linearZBuffer->GetBinHwResId());
						DeviceContext->OMSetRenderTargets(1,&linearDepthBuffer->Surface,NULL);
						DeviceContext->PSSetShaderResources(0,1,&m_ZBuffer);
						PushShader(SHADER_VS_BASE_SCREENVERTEX);
						PushShader(SHADER_PS_LINEAR_DEPTH);
						UpdateConstantBuffer(m_PostProcessConstant,&m_ShaderConstant,sizeof(m_ShaderConstant));
						DeviceContext->PSSetConstantBuffers(10,1,&m_PostProcessConstant);
						FullScreenQuad(Vec2f(1.f,1.f),Vec2f(0.f,0.f));

						rtex = reinterpret_cast<TextureLink*>(m_ssaoBuffer->GetBinHwResId());
						DeviceContext->OMSetRenderTargets(1,&rtex->Surface,NULL);
						//DeviceContext->ClearRenderTargetView( rtex->Surface, ClearColor2 );
						DeviceContext->PSSetShaderResources(0,1,&linearDepthBuffer->ShaderView);
						PushShader(SHADER_VS_BASE_SCREENVERTEX);
						PushShader(SHADER_PS_SSAO);
						UpdateConstantBuffer(m_PostProcessConstant,&m_ShaderConstant,sizeof(m_ShaderConstant));
						DeviceContext->PSSetConstantBuffers(10,1,&m_PostProcessConstant);
						FullScreenQuad(Vec2f(1.f,1.f),Vec2f(0.f,0.f));
						DeviceContext->OMSetDepthStencilState(m_DefaultDS,0);
						DeviceContext->PSSetShaderResources(0,0,NULL);
						DeviceContext->PSSetSamplers(0,1,&m_DefaultSS);
					}
		//			D3DPERF_EndEvent();
				}
		//		D3DPERF_EndEvent();

		*/

#if defined(_PCDX11_)

		//		D3DPERF_BeginEvent(0,L"Forward");
		{
			ID3D11RasterizerState *	m_TempRS;
			D3D11_RASTERIZER_DESC rs;
			rs.AntialiasedLineEnable = FALSE;
			rs.CullMode = D3D11_CULL_NONE;
			rs.DepthBias = 0.f;
			rs.DepthBiasClamp = 0.f;
			rs.DepthClipEnable = TRUE;
			rs.FillMode = D3D11_FILL_WIREFRAME;
			rs.FrontCounterClockwise = FALSE;
			rs.MultisampleEnable = FALSE;
			rs.ScissorEnable = FALSE;
			rs.SlopeScaledDepthBias = 0.f;
			//Device->CreateRasterizerState(&rs,&m_TempRS);
			//DeviceContext->RSSetState(m_TempRS);

			TextureLink * rtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
			GetCommandList()->OMSetRenderTargets(1, &rtex->Surface, m_DepthBuffer);
			float ClearColor2[4] = { 1.0f, 1.f, 1.f, 1.0f }; // red,green,blue,alpha
			GetCommandList()->ClearRenderTargetView(rtex->Surface, ClearColor2);

			rtex = reinterpret_cast<TextureLink*>(m_ssaoBuffer->GetBinHwResId());
			GetCommandList()->PSSetShaderResources(7, 1, &rtex->ShaderView);

			// Setup the viewport
			D3D11_VIEWPORT vp;
			vp.Width = (FLOAT)SizeX;
			vp.Height = (FLOAT)SizeY;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			GetCommandList()->RSSetViewports(1, &vp);

			for (int i = 0; i < gData.m_GraphObjectDA.GetSize(); i++)
			{
				GraphObject * it = gData.m_GraphObjectDA[i];
				it->Draw();
			}

			GetCommandList()->RSSetState(m_DefaultRS);
			//m_TempRS->Release();
		}
		//		D3DPERF_EndEvent();
#endif

//		D3DPERF_BeginEvent(0,L"PostProcess");

		//TextureLink * tex = reinterpret_cast<TextureLink*>(m_lightBuffer->GetBinHwResId());
		TextureLink * hdrtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
		TextureLink * rtex = reinterpret_cast<TextureLink*>(m_RenderTarget->GetBinHwResId());

#if defined(_PCDX11_)
		GetCommandList()->OMSetRenderTargets(1, &rtex->Surface, m_DepthBuffer);
		GetCommandList()->PSSetShaderResources(0, 1, &hdrtex->ShaderView);
		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_COLORGRADING);
		FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));
#endif

		// Setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)SizeX;
		vp.Height = (FLOAT)SizeY;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;


#if defined(_PCDX11)
		GetCommandList()->RSSetViewports(1, &vp);
		GetCommandList()->OMSetRenderTargets(1, &m_BackBuffer, m_DepthBuffer);
		GetCommandList()->PSSetShaderResources(0, 1, &rtex->ShaderView);
#else
		GetCommandList()->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT*>(&vp));
#endif
		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_FXAA);

		FullScreenQuad(Vec2f(1.f, 1.f), Vec2f(0.f, 0.f));

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

#if defined(_PCDX11)
		ID3D11ShaderResourceView* ppSRVNULL[1] = { NULL };
		GetCommandList()->PSSetShaderResources(0, 1, ppSRVNULL);
#endif

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;              // set the time elapsed since the previous frame (in seconds)
		io.DisplaySize.x = SizeX;             // set the current display width
		io.DisplaySize.y = SizeY;             // set the current display height here

		ImGui::NewFrame();
		DrawImGUI();

		GetHAL().PresentFrame();
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

	void DXRenderer::Shut()
	{
		ReleaseAllResources();
#ifdef _PCDX11
		GetCommandList()->ClearState();
		GetCommandList()->Release();
#endif
		GetHAL().Shut();
	}

	void DXRenderer::ReleaseAllResources()
	{
	}

	void DXRenderer::FullScreenQuad(Vec2f scale,Vec2f offset)
	{
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

		GetCommandList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		GetCommandList()->DrawInstanced(4,1,0,0);

#ifdef _PCDX12
		PSO->Release();
#endif
	}

	void DXRenderer::PostProcess()
	{
	}

	void DXRenderer::PushWorldMatrix( Mat4x4* _m )
	{
		Mat4x4 * _mat = (Mat4x4*)(m_VSConstantCache+8);
		_mat[0] = *_m;
	}

	void DXRenderer::PushVertexDeclaration(VertexDeclaration* Decl)
	{
#ifdef _PCDX11
		DXVertexDeclaration* dxbuffer = reinterpret_cast<DXVertexDeclaration*>(Decl);
		GetCommandList()->IASetInputLayout(dxbuffer->GetRes());
		//m_StateCache.VB = dxbuffer;
#endif
	}

	void DXRenderer::PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride)
	{
		DXVertexBuffer* dxbuffer = reinterpret_cast<DXVertexBuffer*>(Buffer);
#ifdef _PCDX11
		ID3D11Buffer * pBuffer = dxbuffer->GetRes();
		UINT _Stride = Stride;
		UINT _Offset = Offset;
		GetCommandList()->IASetVertexBuffers(StreamNumber,1,&pBuffer,&_Stride,&_Offset);
#else
		D3D12_VERTEX_BUFFER_VIEW view = dxbuffer->GetView();
		view.BufferLocation += Offset*Stride;
		view.StrideInBytes = Stride;
		GetCommandList()->IASetVertexBuffers(StreamNumber, 1, &view);
#endif
		m_StateCache.VB = dxbuffer;
	}

	void DXRenderer::PushIndices(IndexBuffer* Buffer,U32 _Fmt)
	{
		if(Buffer)
		{
			DXIndexBuffer* dxbuffer = reinterpret_cast<DXIndexBuffer*>(Buffer);
#ifdef _PCDX11
			GetCommandList()->IASetIndexBuffer(dxbuffer->GetRes(),(_Fmt==FMT_IDX_32)?DXGI_FORMAT_R32_UINT:DXGI_FORMAT_R16_UINT,0);
#else
			GetCommandList()->IASetIndexBuffer(&dxbuffer->GetView());
#endif
			m_StateCache.IB = dxbuffer;
		}

	}

	void DXRenderer::PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount)
	{
	
		int index_count = 0;
		D3D11_PRIMITIVE_TOPOLOGY _primType;
		switch(Type){
		case PRIM_TRIANGLESTRIP:
			_primType=D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			index_count=PrimCount+2;
			break;
		case PRIM_TRIANGLELIST:
			_primType=D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			index_count=PrimCount*3;
			break;
		default:
			return;
		}
		UpdateVSConstants();
		GetCommandList()->IASetPrimitiveTopology( _primType );
	#if defined(_PCDX11)
		GetCommandList()->DrawIndexed(index_count,StartIndex,BaseVertexIndex);
	#elif defined(_PCDX12)
	#endif
	}

	void DXRenderer::PushShader(U32 _ShaderUID)
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));
		GetHAL().SetShaders(Type, SID);
	}

	void DXRenderer::PushMaterial(Material* Mat)
	{
#if defined(_PCDX11)
		Bitmap* bm = NULL;
		bm = Mat->GetBitmap(MTL_STAGE_DIFFUSE);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			GetCommandList()->PSSetShaderResources(0,1,&tex->ShaderView);
		}
		bm = Mat->GetBitmap(MTL_STAGE_NORMAL);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			GetCommandList()->PSSetShaderResources(1,1,&tex->ShaderView);
		}
		bm = Mat->GetBitmap(MTL_STAGE_SPEC);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			GetCommandList()->PSSetShaderResources(2,1,&tex->ShaderView);
		}
#endif
	}

	void DXRenderer::UpdateVSConstants()
	{
#if defined(_PCDX11)
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		GetCommandList()->Map(m_VSConstant,0,D3D11_MAP_WRITE_DISCARD,0,&pMappedResource);
		memcpy(pMappedResource.pData,m_VSConstantCache,VS_CONSTANT_BUFFER_SIZE);
		GetCommandList()->Unmap(m_VSConstant,0);
		GetCommandList()->VSSetConstantBuffers( 0, 1, &m_VSConstant );
#else

		//GetCommandList()->SetGraphicsRootConstantBufferView(0, );
#endif
	}

	void DXRenderer::UpdateConstantBuffer(ID3D11Buffer * _Buffer,void* _DataPtr,U32 _DataSize)
	{
#if defined(_PCDX11)
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		GetCommandList()->Map(_Buffer,0,D3D11_MAP_WRITE_DISCARD,0,&pMappedResource);
		memcpy(pMappedResource.pData,_DataPtr,_DataSize);
		GetCommandList()->Unmap(_Buffer,0);
#endif
	}
	
};

#endif //_PC

