#pragma once

#include <D3D12Interop.h>
#include <D3D12HALBuffers.h>
#include <D3D12HALConstantBuffer.h>
#include <D3D12HALDescriptorHeaps.h>
#include <D3D12HALSamplerState.h>

using namespace Microsoft::WRL;

#define MAX_SRVS 48
#define MAX_CBS 16
#define MAX_SAMPLERS 16
#define MAX_UAVS 8

#define SRV_CBV_DESCRIPTOR_TABLE_OFFSET	(MAX_SRVS+MAX_CBS)

class D3D12HAL
{
public:
	const char * GetName() { return "D3D12"; }
	void Init(int sizeX,int sizeY, sys::Renderer* owner);
	void Shut();

	void PresentFrame();

	VertexDeclaration *	CreateVertexDecl(VertexElement* Decl, U32 _ShaderUID);
	void CreateShaderResource(ID3DBlob * pCode, UINT Type, UINT SID);
	void CreateTexture(Bitmap * _Bm);
	VertexBuffer* CreateVertexBuffer(U32 _Size, U32 _Usage, void* _Datas);
	IndexBuffer* CreateIndexBuffer(U32 _Size, U32 _Usage, U32 _Fmt, void* _Datas);
	ConstantBuffer* CreateConstantBuffer(U32 _Size);

	void InitShaders();
	void SetPrimitiveTopology(PrimitiveType Topology);
	void SetVertexDeclaration(VertexDeclaration* Decl);
	void SetIndices(IndexBuffer* Buffer, U32 _Fmt);
	void SetStreamSource(U32 StreamNumber, VertexBuffer* Buffer, U32 Offset, U32 Stride);
	void SetShaders(UINT Type, UINT SID);
	ID3DBlob * GetShaderBlob(UINT Type, UINT SID)
	{
		if (Type == SHADER_TYPE_VERTEX)
		{
			return m_VSDABlob[SID];
		}
		if (Type == SHADER_TYPE_PIXEL)
		{
			return m_PSDABlob[SID];
		}
		return NULL;
	}

	D3D12DescriptorHeap& GetSrvHeap() { return m_SrvHeap; }
	D3D12DescriptorHeap& GetSamplerHeap() { return m_SamplerHeap; }
	ID3D12Device* GetDevice() { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList.Get(); }
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetPipelineState() { return m_CurrentPSO; }

	// Graphics command list
	void SetAndClearRenderTarget();
	void SetDepthStencilState(DepthStencilDesc& Desc);
	void SetRasterizerState(RasterizerDesc& Desc);
	void SetBlendState(BlendDesc& desc);
	void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler);
	void SetConstantBuffer(U32 Slot, EShaderType Type, ConstantBuffer* CBV);
	void SetShaderResource(U32 Slot, EShaderType Type, sys::TextureLink* View);
	void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	void SetViewports(D3D11_VIEWPORT& Viewport);

	D3D12_CPU_DESCRIPTOR_HANDLE& GetCurrentBackBufferView() {
		return m_RenderTargetsView[m_FrameIndex];
	}

private:
	ComPtr<IDXGIFactory4>				m_DxgiFactory;
	ComPtr<IDXGISwapChain3>				m_SwapChain;
	ComPtr<ID3D12Device>				m_Device;
	ComPtr<ID3D12CommandAllocator>		m_CommandAllocator;
	ComPtr<ID3D12CommandQueue>			m_CommandQueue;
	ComPtr<ID3D12RootSignature>			m_RootSignature;
	ComPtr<ID3D12GraphicsCommandList>	m_CommandList;
	ComPtr<ID3D12Fence>					m_SyncFence;

	UINT								m_FrameIndex = 0;
	static const UINT					m_BufferCount = 2;
	UINT								m_FrameCount = 1;

	D3D12DescriptorHeap					m_SRVDynamicHeap;
	D3D12DescriptorHeap					m_SamplerDynamicHeap;

	// Descriptor heaps by types
	D3D12DescriptorHeap					m_SrvHeap;
	D3D12DescriptorHeap					m_SamplerHeap;
	ComPtr<ID3D12DescriptorHeap>		m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap>		m_DsvHeap;

	// je ne sais pas ce que c'est pour l'instant
	UINT								m_RtvDescriptorSize;
	ComPtr<ID3D12Resource>				m_RenderTargets[m_BufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RenderTargetsView[m_BufferCount];
	UINT								m_DsvDescriptorSize;
	ComPtr<ID3D12Resource>				m_DepthStencil;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_DepthStencilView;

	// Current PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_CurrentPSO;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_CurrentSRV[8][MAX_SRVS];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_CurrentCBV[8][MAX_CBS];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_CurrentSampler[8][MAX_SAMPLERS];
	D3D12SamplerStateCache				m_SamplerStateCache;

	//
	D3D12_SHADER_BYTECODE			m_VSDA[SHADER_VS_COUNT];
	D3D12_SHADER_BYTECODE			m_PSDA[SHADER_PS_COUNT];
	D3D12_SHADER_BYTECODE			m_CSDA[SHADER_CS_COUNT];
	ID3DBlob *						m_VSDABlob[SHADER_VS_COUNT];
	ID3DBlob *						m_PSDABlob[SHADER_PS_COUNT];
	ID3DBlob *						m_CSDABlob[SHADER_CS_COUNT];
	ID3D12ShaderReflection *		m_VSDAReflection[SHADER_VS_COUNT];
	ID3D12ShaderReflection *		m_PSDAReflection[SHADER_PS_COUNT];
	ID3D12ShaderReflection *		m_CSDAReflection[SHADER_CS_COUNT];

	D3D12VertexDeclarationDA		m_InputLayoutDA;
};
