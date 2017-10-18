#if defined(_ANDROID)

#include <nv_shader/nv_shader.h>
#include <nv_time/nv_time.h>
#include <nv_thread/nv_thread.h>
#include <nv_event/nv_event.h>
#include <nv_math/nv_math.h>
#include <nv_math/nv_matrix.h>

#include <AndroidMain.h>
#include <AndroidRenderer.h>

using namespace sys;

int AndroidRenderer::Init()
{
MESSAGE("AndroidRenderer::Init");
	InitShaders();
}

bool AndroidRenderer::InitStaticDatas()
{
MESSAGE("AndroidRenderer::InitStaticDatas");
}

void AndroidRenderer::InitShaders()
{
	Renderer::InitShaders();
}

void AndroidRenderer::Shut()
{
MESSAGE("AndroidRenderer::Shut");
}

Bool AndroidRenderer::Resume()
{
MESSAGE("AndroidRenderer::Resume");
	if (!NVEventEGLInit())
	{
		MESSAGE( "FAILED NvEventEGLInit()" );
		return FALSE;
	}

	NVEventEGLMakeCurrent();
	MESSAGE( "GL_VENDOR: %s", glGetString(GL_VENDOR) );
	MESSAGE( "GL_VERSION: %s", glGetString(GL_VERSION) );
	MESSAGE( "GL_RENDERER: %s", glGetString(GL_RENDERER) );
	return TRUE;
}

void AndroidRenderer::Pause()
{
MESSAGE("AndroidRenderer::Pause");
	NVEventEGLUnmakeCurrent();
}

void AndroidRenderer::ReleaseAllResources()
{
MESSAGE("AndroidRenderer::ReleaseAllResources");
}

void AndroidRenderer::MainLoop()
{
	if(sys::android::m_uiVisible==false)
		return;

	m_Camera->Update();

	glClearColor(0.15f, 0.05f, 0.15f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

	//D3DPERF_BeginEvent(0,L"Forward");
	{
		/*
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
		*/
		glViewport(0,0,SizeX,SizeY);

		for(int i=0;i<gData.m_GraphObjectDA.GetSize();i++)
		{
			GraphObject * it = gData.m_GraphObjectDA[i];
			it->Draw();
		}
	}

	// Present
	glFinish();
	NVEventEGLSwapBuffers();
}

#endif //_ANDROID
