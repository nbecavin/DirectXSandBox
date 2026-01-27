#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <PrimitiveBuffers.h>
#include <RendererShader.h>
#include <RendererPipelineState.h>
#include <Material.h>
#include <Camera.h>
#include <imgui.h>
#include <ShaderConstants.h>
#include <GpuScene.h>
#include <..\..\Shaders\ShaderRegs.h>
#include <RenderHAL.h>

class RenderGraph;

namespace sys {

	class Renderer
	{
	public:

		virtual int	Init();
		virtual bool InitStaticDatas();
		virtual void Shut();
		virtual void BeginFrame(float dTime);
		virtual void MainLoop();
		virtual void InitShaders();
		virtual void SetSize(U32 _SizeX,U32 _SizeY) { SizeX=_SizeX; SizeY=_SizeY; }
		virtual Vec2f GetSize() { return Vec2f((float)SizeX, (float)SizeY); }
		virtual U32 GetSizeX() { return SizeX; }
		virtual U32 GetSizeY() { return SizeY; }

		virtual GlobalParameters GetGlobalParameters() const { return m_GlobalParams; }
		virtual void SetGlobalParameters(GlobalParameters p) {
			m_GlobalParams = p;
		}

		// ImGui integration
		virtual void InitImGUI();
		virtual void DrawImGUI();

		// Render Graph
		RenderGraph* GetRenderGraph() const {
			return m_RenderGraph;
		}

		RenderHAL* GetHAL() { return m_HAL.get(); }

		// Set current scene camera
				void	SetCamera(Camera * _Cam) { m_Camera=_Cam; }
		inline	Camera* GetCamera() const { return m_Camera; }
		
	protected:
		int		SizeX, SizeY;

		Bitmap				*	m_RenderTarget;
		Bitmap				*	m_HdrRenderTarget;

		Bitmap				*	m_gBuffer[4];
		Bitmap				*	m_linearZBuffer;
		Bitmap				*	m_lightBuffer;
		Bitmap				*	m_localLightBuffer;
		Bitmap				*	m_shadowBuffer;
		Bitmap				*	m_ssaoBuffer;

		// Viewports and camera
		Camera				*	m_Camera;

		// ImGui data
		VertexDeclaration* m_ImGuiVertexDeclaration = nullptr;
		VertexBuffer* m_ImGuiVB = nullptr;
		IndexBuffer* m_ImGuiIB = nullptr;

		GlobalParameters		m_GlobalParams;

		// Some global buffers... yeah it's bad
		ConstantBuffer* m_GlobalConstant;
		ConstantBuffer* m_CameraConstant;

		RenderGraph* m_RenderGraph;
		std::unique_ptr<RenderHAL> m_HAL;

	};

};

template<typename T>
T* GetHALPtr() {
	return gData.Rdr->GetHAL()->AsPtr<T>();
}

#endif //__RENDERER_H__
