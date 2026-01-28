#ifndef __RENDERHAL_HH__
#define __RENDERHAL_HH__

#include <PrimitiveBuffers.h>
#include <Bitmap.h>
#include <ShaderConstants.h>
#include <RendererShader.h>
#include <RendererPipelineState.h>

namespace sys
{
	struct TextureLink
	{
		union {
			ID3D12Resource* Resource12;
		};
		D3D12_CPU_DESCRIPTOR_HANDLE m_RTV;
		D3D12_CPU_DESCRIPTOR_HANDLE m_SRV;
		D3D12_CPU_DESCRIPTOR_HANDLE m_UAV;
	};
};

struct Vertex2D
{
	float		x, y, z;
	float		u, v;
};

typedef DynArray<VertexBuffer*, 2048>		VertexBufferDA;
typedef DynArray<IndexBuffer*, 2048>		IndexBufferDA;

namespace sys {

	class RenderHAL// : public Renderer
	{
	public:

		template<typename T> T* AsPtr() { return (T*)this; }

		virtual const char* GetName() = 0;

		virtual void Init(int sizeX, int sizeY, sys::Renderer* owner);
		virtual void Shut();

		virtual bool InitStaticDatas();
		virtual void ReleaseAllResources();
		virtual void MainLoop();

		U32 GetFrameIndex() const { return m_FrameIndex; }
		void AdvanceFrameIndex() { m_FrameIndex++; }

		// Push Event marker on current CommandList
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

		virtual VertexBuffer* CreateVertexBuffer(U32 _Size, U32 _Usage, void* _Datas) = 0;
		virtual IndexBuffer* CreateIndexBuffer(U32 _Size, U32 _Usage, U32 _Fmt, void* _Datas) = 0;
		virtual VertexDeclaration* CreateVertexDecl(VertexElement* Decl) = 0;
		virtual void CreateTexture(Bitmap* _Bmap) = 0;
		virtual Buffer* CreateBuffer() = 0;
		virtual ConstantBuffer* CreateConstantBuffer(U32 _Size) = 0;

		virtual void SetScissorRect(U32 left, U32 right, U32 top, U32 bottom) = 0;

		virtual void BindGraphicPipelineState(ShaderKernel* VS, ShaderKernel* PS) = 0;
		virtual void BindComputePipelineState(ShaderKernel* CS) = 0;

		virtual void SetVertexDeclaration(VertexDeclaration* Decl) = 0;
		virtual void SetStreamSource(U32 StreamNumber, VertexBuffer* Buffer, U32 Offset, U32 Stride) = 0;
		virtual void SetIndices(IndexBuffer* Buffer, U32 _Fmt) = 0;

		virtual void SetConstantBuffer(U32 Slot, EShaderType Type, ConstantBuffer* CBV) = 0;
		virtual void SetShaderResource(U32 Slot, EShaderType Type, Bitmap* Texture);
		virtual void SetUAV(U32 Slot, Bitmap* Texture);
		virtual void SetDepthStencilState(DepthStencilDesc& Desc) = 0;
		virtual void SetRasterizerState(RasterizerDesc& Desc) = 0;
		virtual void SetPrimitiveTopology(PrimitiveType Topology) = 0;

		virtual void DrawInstanced(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation) = 0;
		virtual void DrawIndexedInstanced(UINT IndexCount, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation) = 0;
		virtual void Dispatch(UINT ThreadGroupCountX, UINT ThreadGroupCountY, UINT ThreadGroupCountZ) = 0;
		virtual void DispatchRays(UINT DispatchRaysX, UINT DispatchRaysY, UINT DispatchRaysZ) = 0;

		virtual void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler) = 0;
		virtual void SetBlendState(BlendDesc& desc) = 0;

		virtual void BuildAccelerationStructure() = 0;
		virtual void BuildTLAS() = 0;

		virtual ShaderKernel* CreateKernel(const char* src, const char* epoint, EShaderType type);

		virtual void InitShaders();

		void FullScreenQuad(Vec2f scale, Vec2f offset);

		void PostProcess();

	protected:
		int	SizeX, SizeY;
		int m_FrameIndex;

		VertexBuffer* m_FullscreenQuadVB;
		VertexDeclaration* m_ScreenVertexDecl;

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

#endif //__RENDERHAL_HH__
