#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <PrimitiveBuffers.h>
#include <RendererShader.h>
#include <RendererPipelineState.h>
#include <Material.h>
#include <Camera.h>
#include <imgui.h>
#include <ShaderConstants.h>

namespace sys {

	class Renderer
	{
	public:

		virtual int	Init();
		virtual bool InitStaticDatas();
		virtual void Shut() = 0;
		virtual void BeginFrame(float dTime);
		virtual void MainLoop() = 0;
		virtual void InitShaders();
		virtual void SetSize(U32 _SizeX,U32 _SizeY) { SizeX=_SizeX; SizeY=_SizeY; }
		virtual Vec2f GetSize() { return Vec2f(SizeX, SizeY); }
		virtual U32 GetSizeX() { return SizeX; }
		virtual U32 GetSizeY() { return SizeY; }

		virtual void ProfileBeginEvent(U32 _ColorRGBA, const char* _Message) = 0;
		virtual void ProfileBeginEventArgs(U32 _ColorRGBA, const char* _Format, ...)
		{
			va_list args, args_copy;
			va_start(args, _Format);
			va_copy(args_copy, args);
			const auto len = std::vsnprintf(nullptr, 0, _Format, args) + 1;
			std::string str(len, ' ');
			std::vsnprintf(&str.front(), len, _Format, args_copy);
			va_end(args);
			va_end(args_copy);
			ProfileBeginEvent(_ColorRGBA, str.c_str());
		}
		virtual void ProfileEndEvent() = 0;

		// ImGui integration
		virtual void InitImGUI();
		virtual void DrawImGUI();

		// Set current scene camera
				void	SetCamera(Camera * _Cam) { m_Camera=_Cam; }
		inline	Camera* GetCamera() const { return m_Camera; }
		
		//
		virtual void UpdateGlobalConstant();
		virtual void SetGlobalConstant(EShaderType Type);

		//
		// Vertex/Index buffers
		virtual VertexBuffer *		CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas);
		virtual IndexBuffer *		CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas);
		virtual VertexDeclaration *	CreateVertexDecl(VertexElement* Decl,U32 _ShaderUID);
		virtual void				CreateTexture(Bitmap * _Bmap) = 0;
		virtual ConstantBuffer *	CreateConstantBuffer(U32 _Size) = 0;

		virtual GlobalConstant* GetGlobalConstantData() { return &m_GlobalConstantData; }
		virtual void	PushWorldMatrix(Mat4x4* _m);

		virtual void	PushShader( U32 _ShaderUID );
		virtual void	PushVertexDeclaration(VertexDeclaration* Decl) {}
		virtual void	PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride) {}
		virtual void	PushIndices(IndexBuffer* Buffer,U32 _Fmt=FMT_IDX_16) {}
		virtual void	PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount) {}
		virtual void	PushMaterial(Material* Mat) {}
		virtual void	RegisterShaderFromSourceFile(U32 _SUID,const char* src,const char* epoint) = 0;

		// Graphics command list
		virtual void SetBlendState(BlendDesc& desc) = 0;
		virtual void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler) = 0;
		virtual void SetConstantBuffer(U32 Slot, EShaderType Type, ConstantBuffer* CBV) = 0;
		virtual void SetShaderResource(U32 Slot, EShaderType Type, Bitmap* Texture) = 0;
		virtual void SetDepthStencilState(DepthStencilDesc& Desc) = 0;
		virtual void SetRasterizerState(RasterizerDesc& Desc) = 0;
		virtual void SetPrimitiveTopology(PrimitiveType Topology) = 0;
		virtual void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation) = 0;

	protected:
		int		SizeX, SizeY;

		int						m_FrameIndex;

		VertexBuffer		*	m_FullscreenQuadVB;
		VertexDeclaration	*	m_ScreenVertexDecl;

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

		// Some global buffers... yeah it's bad
		GlobalConstant			m_GlobalConstantData;
		ConstantBuffer*			m_GlobalConstant;
		ConstantBuffer*			m_CameraConstant;
	};

};

#endif //__RENDERER_H__
