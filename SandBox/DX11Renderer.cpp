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

		float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; // red,green,blue,alpha
		DeviceContext->ClearRenderTargetView( m_BackBuffer, ClearColor );
		DeviceContext->ClearDepthStencilView( m_DepthBuffer, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.f, 0 );

		//une view et une proj de base
		XMMATRIX m,proj,view;
		XMVECTOR eye = XMVectorSet(0,1,-5,0);
		XMVECTOR at = XMVectorSet(0,1,0,0);
		XMVECTOR up = XMVectorSet(0,1,0,0);
		view = XMMatrixLookAtRH(eye,at,up);

		view = *reinterpret_cast<XMMATRIX*>(&m_Camera->GetViewMatrix());

		proj = XMMatrixPerspectiveRH(.25,.25f/((float)SizeX/(float)SizeY),0.1,1000);

		CameraConstant camCst;
		camCst.ProjMatrix = XMMatrixTranspose(proj);
		camCst.ViewMatrix = XMMatrixTranspose(view);
		XMVECTOR det;
		camCst.InvProjMatrix = XMMatrixTranspose(XMMatrixInverse(&det,proj));
		//camCst.InvViewProjMatrix = ;
		UpdateConstantBuffer(m_CameraConstant,&camCst,sizeof(camCst));

		XMMATRIX * _mat = (XMMATRIX*)(m_VSConstantCache);
		_mat[0] = XMMatrixTranspose(proj);
		_mat[1] = XMMatrixTranspose(view);
		Vec4f * _vec = (Vec4f*)(m_VSConstantCache);
		_vec[EYE_CST] = m_Camera->GetWorldPosition();

		// No geometry shader
		DeviceContext->GSSetShader(NULL,NULL,0);

		DeviceContext->RSSetState(m_DefaultRS);
		DeviceContext->OMSetDepthStencilState(m_DefaultDS,0);
		DeviceContext->PSSetSamplers(0,1,&m_DefaultSS);
		DeviceContext->PSSetSamplers(1,1,&m_DefaultSS);
		DeviceContext->PSSetSamplers(2,1,&m_DefaultSS);

		DeviceContext->VSSetConstantBuffers(8,1,&m_SHHemisphere);
		DeviceContext->PSSetConstantBuffers(8,1,&m_SHHemisphere);

		DeviceContext->VSSetConstantBuffers(9,1,&m_CameraConstant);
		DeviceContext->PSSetConstantBuffers(9,1,&m_CameraConstant);

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
			DeviceContext->OMSetRenderTargets(1,&rtex->Surface,m_DepthBuffer);
			float ClearColor2[4] = { 1.0f, 1.f, 1.f, 1.0f }; // red,green,blue,alpha
			DeviceContext->ClearRenderTargetView( rtex->Surface, ClearColor2 );

			rtex = reinterpret_cast<TextureLink*>(m_ssaoBuffer->GetBinHwResId());
			DeviceContext->PSSetShaderResources(7,1,&rtex->ShaderView);

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
				it->Draw();
			}

			DeviceContext->RSSetState(m_DefaultRS);
			//m_TempRS->Release();
		}
//		D3DPERF_EndEvent();

//		D3DPERF_BeginEvent(0,L"PostProcess");

		//TextureLink * tex = reinterpret_cast<TextureLink*>(m_lightBuffer->GetBinHwResId());
		TextureLink * hdrtex = reinterpret_cast<TextureLink*>(m_HdrRenderTarget->GetBinHwResId());
		TextureLink * rtex = reinterpret_cast<TextureLink*>(m_RenderTarget->GetBinHwResId());
			
		DeviceContext->OMSetRenderTargets( 1, &rtex->Surface, m_DepthBuffer );
		DeviceContext->PSSetShaderResources(0,1,&hdrtex->ShaderView);
		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_COLORGRADING);
		FullScreenQuad(Vec2f(1.f,1.f),Vec2f(0.f,0.f));

		DeviceContext->OMSetRenderTargets( 1, &m_BackBuffer, m_DepthBuffer );
		DeviceContext->PSSetShaderResources(0,1,&rtex->ShaderView);
		PushShader(SHADER_VS_BASE_SCREENVERTEX);
		PushShader(SHADER_PS_POSTPROC_FXAA);
		FullScreenQuad(Vec2f(1.f,1.f),Vec2f(0.f,0.f));

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
		DeviceContext->PSSetShaderResources(0,1,ppSRVNULL);

		SwapChain->Present(1,0);
	}

	void DXRenderer::InitSurface()
	{
	}

	void DXRenderer::InitShaders()
	{
		HRESULT hr;

		memset(m_VSDA,0,sizeof(m_VSDA));
		memset(m_PSDA,0,sizeof(m_PSDA));
		memset(m_VSDABlob,0,sizeof(m_VSDABlob));
		memset(m_PSDABlob,0,sizeof(m_PSDABlob));

		// Register common shaders
		Renderer::InitShaders();
	}

	void DXRenderer::Shut()
	{
		ReleaseAllResources();
		DeviceContext->ClearState();
		DeviceContext->Release();
		Device->Release();
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
		DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		DeviceContext->Draw(4,0);
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
		DXVertexDeclaration* dxbuffer = reinterpret_cast<DXVertexDeclaration*>(Decl);
		DeviceContext->IASetInputLayout(dxbuffer->GetRes());
		//m_StateCache.VB = dxbuffer;
	}

	void DXRenderer::PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride)
	{
		DXVertexBuffer* dxbuffer = reinterpret_cast<DXVertexBuffer*>(Buffer);
		ID3D11Buffer * pBuffer = dxbuffer->GetRes();
		UINT _Stride = Stride;
		UINT _Offset = Offset;
		DeviceContext->IASetVertexBuffers(StreamNumber,1,&pBuffer,&_Stride,&_Offset);
		m_StateCache.VB = dxbuffer;
	}

	void DXRenderer::PushIndices(IndexBuffer* Buffer,U32 _Fmt)
	{
		if(Buffer)
		{
			DXIndexBuffer* dxbuffer = reinterpret_cast<DXIndexBuffer*>(Buffer);
			DeviceContext->IASetIndexBuffer(dxbuffer->GetRes(),(_Fmt==FMT_IDX_32)?DXGI_FORMAT_R32_UINT:DXGI_FORMAT_R16_UINT,0);
			m_StateCache.IB = dxbuffer;
		}
	}

	void DXRenderer::PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount)
	{
	#if defined(_PCDX9)
		D3DPRIMITIVETYPE _primType;
		switch(Type){
		case PRIM_TRIANGLESTRIP:	_primType=D3DPT_TRIANGLESTRIP; break;
		case PRIM_TRIANGLELIST:		_primType=D3DPT_TRIANGLELIST; break;
		default:
			return;
		}
		Device->DrawIndexedPrimitive(_primType,BaseVertexIndex,MinVertexIndex,NumVertices,StartIndex,PrimCount);
	#elif defined(_PCDX11)
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
		DeviceContext->IASetPrimitiveTopology( _primType );
		DeviceContext->DrawIndexed(index_count,StartIndex,BaseVertexIndex);
	#endif
	}

	void DXRenderer::PushShader(U32 _ShaderUID)
	{
		U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
		U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));
		if(Type==SHADER_TYPE_VERTEX)
		{
			DeviceContext->VSSetShader(m_VSDA[SID],NULL,0);
		}
		if(Type==SHADER_TYPE_PIXEL)
		{
			DeviceContext->PSSetShader(m_PSDA[SID],NULL,0);
		}
	}

	void DXRenderer::PushMaterial(Material* Mat)
	{
		Bitmap* bm = NULL;
		bm = Mat->GetBitmap(MTL_STAGE_DIFFUSE);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			DeviceContext->PSSetShaderResources(0,1,&tex->ShaderView);
		}
		bm = Mat->GetBitmap(MTL_STAGE_NORMAL);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			DeviceContext->PSSetShaderResources(1,1,&tex->ShaderView);
		}
		bm = Mat->GetBitmap(MTL_STAGE_SPEC);
		if(bm)
		{
			TextureLink * tex = reinterpret_cast<TextureLink*>(bm->GetBinHwResId());
			DeviceContext->PSSetShaderResources(2,1,&tex->ShaderView);
		}
	}

	void DXRenderer::UpdateVSConstants()
	{
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		DeviceContext->Map(m_VSConstant,0,D3D11_MAP_WRITE_DISCARD,0,&pMappedResource);
		memcpy(pMappedResource.pData,m_VSConstantCache,VS_CONSTANT_BUFFER_SIZE);
		DeviceContext->Unmap(m_VSConstant,0);
		DeviceContext->VSSetConstantBuffers( 0, 1, &m_VSConstant );
	}

	void DXRenderer::UpdateConstantBuffer(ID3D11Buffer * _Buffer,void* _DataPtr,U32 _DataSize)
	{
		D3D11_MAPPED_SUBRESOURCE pMappedResource;
		DeviceContext->Map(_Buffer,0,D3D11_MAP_WRITE_DISCARD,0,&pMappedResource);
		memcpy(pMappedResource.pData,_DataPtr,_DataSize);
		DeviceContext->Unmap(_Buffer,0);
	}
	
};

#endif //_PC

