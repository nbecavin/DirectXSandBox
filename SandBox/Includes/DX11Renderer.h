#ifndef __DX11RENDERER_HH__
#define __DX11RENDERER_HH__

#include <Renderer.h>
#include <ShaderConstants.h>

#if defined(_PCDX11)
#include <D3D11HAL.h>
#endif
#if defined(_PCDX12)
#include <D3D12HAL.h>
#endif

#define USE_D3D12

#if defined(_PCDX11) || defined(_PCDX12)

struct Vertex2D
{
	float		x,y,z;
	float		u,v;
};

typedef DynArray<VertexBuffer*, 2048>		VertexBufferDA;
typedef DynArray<IndexBuffer*, 2048>		IndexBufferDA;

namespace sys {

	struct TextureLink
	{
		union {
			ID3D11Texture2D			* Tex2D;
			ID3D11Resource			* Resource;
		};
		ID3D11ShaderResourceView	* ShaderView;
		ID3D11RenderTargetView		* Surface;
	};

	class DXRenderer : public Renderer
	{
	public:
		virtual int  Init();
		virtual bool InitStaticDatas();
		virtual void Shut();
		virtual void ReleaseAllResources();
		virtual void MainLoop();

		virtual VertexBuffer *		CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas);
		virtual IndexBuffer *		CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas);
		virtual VertexDeclaration *	CreateVertexDecl(VertexElement* Decl, U32 _ShaderUID)
		{
			return GetHAL().CreateVertexDecl(Decl, _ShaderUID);
		}
		virtual void				CreateTexture(Bitmap * _Bmap);

		virtual void	PushWorldMatrix( Mat4x4* _m );

		virtual void	PushShader(U32 _ShaderUID);
		virtual void	PushVertexDeclaration(VertexDeclaration* Decl);
		virtual void	PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride);
		virtual void	PushIndices(IndexBuffer* Buffer,U32 _Fmt);
		virtual void	PushMaterial(Material* Mat);

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

		virtual void PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount);

		virtual void	RegisterShaderFromSourceFile(U32 _SUID,const char* src,const char* epoint);

		virtual void InitShaders();
				void InitSurface();

				void FullScreenQuad(Vec2f scale,Vec2f offset);

				void PostProcess();

		ID3DBlob *				GetShaderBlob(U32 _ShaderUID);

#ifdef _PCDX11
		D3D11HAL				m_D3D11HAL;
		D3D11HAL&				GetD3D11HAL() { return m_D3D11HAL; }
#endif
#ifdef _PCDX12
		D3D12HAL				m_D3D12HAL;
		D3D12HAL&				GetD3D12HAL() { return m_D3D12HAL; }
#endif

#if defined(USE_D3D12) && defined(_PCDX12)
		D3D12HAL&				GetHAL() { return m_D3D12HAL; }
#else
		D3D11HAL&				GetHAL() { return m_D3D11HAL; }
#endif

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
			VertexBuffer *	VB;
			IndexBuffer	*	IB;
		};
		StateCache					m_StateCache;

		void				UpdateConstantBuffer(ID3D11Buffer * _Buffer,void* _DataPtr,U32 _DataSize);
		void				UpdateVSConstants();
	};

	//extern Vec4f				m_VSConstantCache[VS_CONSTANT_MAX_COUNT];

};

#endif

#endif //__DX11RENDERER_HH__
