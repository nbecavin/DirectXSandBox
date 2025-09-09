#ifndef __DXRENDERER_HH__
#define __DXRENDERER_HH__

#define GET_RDR_INSTANCE()	((sys::DXRenderer*)gData.Rdr)

#include <Renderer.h>
#include <ShaderConstants.h>

namespace sys
{
	struct TextureLink
	{
		union {
			ID3D12Resource* Resource12;
		};
		D3D12_RENDER_TARGET_VIEW_DESC* m_D3D12RTV;
		D3D12_CPU_DESCRIPTOR_HANDLE m_D3D12SRVcpu;
		D3D12_GPU_DESCRIPTOR_HANDLE m_D3D12SRVgpu;
	};
};

#include <D3D12HAL.h>

struct Vertex2D
{
	float		x, y, z;
	float		u, v;
};

typedef DynArray<VertexBuffer*, 2048>		VertexBufferDA;
typedef DynArray<IndexBuffer*, 2048>		IndexBufferDA;

namespace sys {

	class DXRenderer : public Renderer
	{
	public:
		virtual int  Init();
		virtual bool InitStaticDatas();
		virtual void Shut();
		virtual void ReleaseAllResources();
		virtual void MainLoop();

		// Push Event marker on current CommandList
		virtual void ProfileBeginEvent(U32 _ColorRGBA, const char* _Message)
		{
			WCHAR wString[129];
			mbstowcs(wString, _Message, 128);
			PIXBeginEvent(GetHAL().GetCommandList(), _ColorRGBA, wString);
		}
		virtual void ProfileEndEvent()
		{
			PIXEndEvent(GetHAL().GetCommandList());
		}

		virtual VertexBuffer* CreateVertexBuffer(U32 _Size, U32 _Usage, void* _Datas);
		virtual IndexBuffer* CreateIndexBuffer(U32 _Size, U32 _Usage, U32 _Fmt, void* _Datas);
		virtual VertexDeclaration* CreateVertexDecl(VertexElement* Decl, U32 _ShaderUID)
		{
			return GetHAL().CreateVertexDecl(Decl, _ShaderUID);
		}
		virtual void				CreateTexture(Bitmap* _Bmap);
		virtual ConstantBuffer* CreateConstantBuffer(U32 _Size)
		{
			return GetHAL().CreateConstantBuffer(_Size);
		}

		virtual void SetScissorRect(U32 left, U32 right, U32 top, U32 bottom)
		{
			return GetHAL().SetScissorRect(left, right, top, bottom);
		}

		virtual void	PushShader(U32 _ShaderUID);
		virtual void	PushVertexDeclaration(VertexDeclaration* Decl);
		virtual void	PushStreamSource(U32 StreamNumber, VertexBuffer* Buffer, U32 Offset, U32 Stride);
		virtual void	PushIndices(IndexBuffer* Buffer, U32 _Fmt);
		virtual void	PushMaterial(Material* Mat);

		virtual void SetConstantBuffer(U32 Slot, EShaderType Type, ConstantBuffer* CBV)
		{
			GetHAL().SetConstantBuffer(Slot, Type, CBV);
		}
		virtual void SetShaderResource(U32 Slot, EShaderType Type, Bitmap* Texture);
		virtual void SetDepthStencilState(DepthStencilDesc& Desc)
		{
			GetHAL().SetDepthStencilState(Desc);
		}
		virtual void SetRasterizerState(RasterizerDesc& Desc)
		{
			GetHAL().SetRasterizerState(Desc);
		}
		virtual void SetPrimitiveTopology(PrimitiveType Topology);
		void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
		{
			GetHAL().DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
		}
		void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler)
		{
			GetHAL().SetSampler(Slot, Type, Sampler);
		}
		void SetBlendState(BlendDesc& desc)
		{
			GetHAL().SetBlendState(desc);
		}

		virtual void PushDrawIndexed(PrimitiveType Type, U32 BaseVertexIndex, U32 MinVertexIndex, U32 NumVertices, U32 StartIndex, U32 PrimCount);

		virtual void	RegisterShaderFromSourceFile(U32 _SUID, const char* src, const char* epoint);

		virtual void InitShaders();
		void InitSurface();

		void FullScreenQuad(Vec2f scale, Vec2f offset);

		void PostProcess();

		ID3DBlob* GetShaderBlob(U32 _ShaderUID);

		D3D12HAL				m_D3D12HAL;
		D3D12HAL& GetD3D12HAL() { return m_D3D12HAL; }
		D3D12HAL& GetHAL() { return m_D3D12HAL; }

	private:
		VertexBufferDA					m_VertexBufferDA;
		IndexBufferDA					m_IndexBufferDA;

		DepthStencilDesc				m_DefaultDS;
		DepthStencilDesc				m_DSS_NoZWrite;
		RasterizerDesc					m_DefaultRS;
		SamplerDesc						m_DefaultSS;
		SamplerDesc						m_NoBilinearSS;

		struct StateCache
		{
			VertexBuffer* VB;
			IndexBuffer* IB;
		};
		StateCache					m_StateCache;

		HMODULE m_PixModule = nullptr;
	};

};

#endif //__DXRENDERER_HH__
