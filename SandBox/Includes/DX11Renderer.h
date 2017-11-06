#ifndef __DX11RENDERER_HH__
#define __DX11RENDERER_HH__

#include <Renderer.h>
#include <ShaderConstants.h>

#if defined(_PCDX12)
#include <D3D12HAL.h>
#elif defined(_PCDX11)
#include <D3D11HAL.h>
#endif
#include <DXBuffers.h>

#if defined(_PCDX11) || defined(_PCDX12)

struct Vertex2D
{
	float		x,y,z;
	float		u,v;
};

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
		virtual VertexDeclaration *	CreateVertexDecl(VertexElement* Decl,U32 _ShaderUID);
		virtual void				CreateTexture(Bitmap * _Bmap);

		virtual void	PushWorldMatrix( Mat4x4* _m );

		virtual void	PushShader(U32 _ShaderUID);
		virtual void	PushVertexDeclaration(VertexDeclaration* Decl);
		virtual void	PushStreamSource(U32 StreamNumber,VertexBuffer* Buffer,U32 Offset,U32 Stride);
		virtual void	PushIndices(IndexBuffer* Buffer,U32 _Fmt);
		virtual void	PushMaterial(Material* Mat);

		virtual void	PushDrawIndexed(PrimitiveType Type,U32 BaseVertexIndex,U32 MinVertexIndex,U32 NumVertices,U32 StartIndex,U32 PrimCount);

		virtual void	RegisterShaderFromSourceFile(U32 _SUID,const char* src,const char* epoint);

		virtual void InitShaders();
				void InitSurface();

				void FullScreenQuad(Vec2f scale,Vec2f offset);

				void PostProcess();

#ifdef _PCDX12
		ID3D12Device *			GetDevice() { return GetHAL().GetDevice(); }
		ID3D12GraphicsCommandList *	GetCommandList() { return GetHAL().GetCommandList(); }
#else
		ID3D11Device *			GetDevice() { return GetHAL().GetDevice(); }
		ID3D11DeviceContext *	GetCommandList() { return GetHAL().GetImmediateDeviceContext(); }
#endif
		ID3DBlob *				GetShaderBlob(U32 _ShaderUID);

	private:
#ifdef _PCDX12
		D3D12HAL							m_HAL;
		D3D12HAL&						GetHAL() { return m_HAL; }
#else
		D3D11HAL							m_HAL;
		D3D11HAL&						GetHAL() { return m_HAL; }
#endif

		// Surfaces
		ID3D11RenderTargetView *		m_BackBuffer;
		ID3D11DepthStencilView *		m_DepthBuffer;

		DXVertexBufferDA				m_VertexBufferDA;
		DXIndexBufferDA					m_IndexBufferDA;
		DXVertexDeclarationDA			m_VertexDeclarationDA;
		DXVertexShaderDA				m_VertexShaderDA;
		DXPixelShaderDA					m_PixelShaderDA;

		ID3D11RasterizerState *			m_DefaultRS;
		ID3D11DepthStencilState *		m_DefaultDS;
		ID3D11DepthStencilState *		m_DSS_NoZWrite;
		ID3D11SamplerState *			m_DefaultSS;
		ID3D11SamplerState *			m_NoBilinearSS;
		ID3D11ShaderResourceView *		m_ZBuffer;

		struct StateCache
		{
			DXVertexBuffer	*	VB;
			DXIndexBuffer	*	IB;
		};
		StateCache					m_StateCache;

		#define VS_CONSTANT_MAX_COUNT	256
		#define VS_CONSTANT_BUFFER_SIZE	(VS_CONSTANT_MAX_COUNT*sizeof(Vec4f))
		ID3D11Buffer *					m_VSConstant;
	
		ID3D11Buffer *					m_SHHemisphere;

		ID3D11Buffer *					m_CameraConstant;
		ID3D11Buffer *					m_PostProcessConstant;

		void				UpdateConstantBuffer(ID3D11Buffer * _Buffer,void* _DataPtr,U32 _DataSize);
		void				UpdateVSConstants();
	};

	extern Vec4f				m_VSConstantCache[VS_CONSTANT_MAX_COUNT];

};

#endif

#endif //__DX11RENDERER_HH__
