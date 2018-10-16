#pragma once

#include <D3D12Interop.h>
#include <D3D12HALBuffers.h>
#include <D3D12HALConstantBuffer.h>

using namespace Microsoft::WRL;

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

	ID3D12Device* GetDevice() { return m_Device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return m_CommandList.Get(); }
	D3D12_GRAPHICS_PIPELINE_STATE_DESC& GetPipelineState() { return m_CurrentPSO; }

	// Graphics command list
	void SetAndClearRenderTarget();
	void SetDepthStencilState(DepthStencilDesc& Desc);
	void SetRasterizerState(RasterizerDesc& Desc);
	inline void SetBlendState(BlendDesc& desc);
	inline void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler);
	inline void SetShaderResource(U32 Slot, EShaderType Type, ID3D11ShaderResourceView* View);
	inline void DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation);
	inline void SetViewports(D3D11_VIEWPORT& Viewport);

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

	// Descriptor heaps by types
	ComPtr<ID3D12DescriptorHeap>		m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap>		m_DsvHeap;
	ComPtr<ID3D12DescriptorHeap>		m_SrvHeap;

	// je ne sais pas ce que c'est pour l'instant
	UINT								m_RtvDescriptorSize;
	ComPtr<ID3D12Resource>				m_RenderTargets[m_BufferCount];
	D3D12_CPU_DESCRIPTOR_HANDLE			m_RenderTargetsView[m_BufferCount];
	UINT								m_DsvDescriptorSize;
	ComPtr<ID3D12Resource>				m_DepthStencil;
	D3D12_CPU_DESCRIPTOR_HANDLE			m_DepthStencilView;

	// Current PSO
	D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_CurrentPSO;

	D3D12ConstantBuffer					m_GlobalConstantBuffer;

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

inline void D3D12HAL::SetViewports(D3D11_VIEWPORT& Vp)
{
	m_CommandList->RSSetViewports(1, reinterpret_cast<D3D12_VIEWPORT*>(&Vp));
	D3D12_RECT rect;
	rect.left = Vp.TopLeftX;
	rect.top = Vp.TopLeftY;
	rect.right = Vp.TopLeftX + Vp.Width;
	rect.bottom = Vp.TopLeftY + Vp.Height;
	m_CommandList->RSSetScissorRects(1, &rect);
}

inline void D3D12HAL::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	ID3D12PipelineState* PSO;
	m_CurrentPSO.pRootSignature = m_RootSignature.Get();
	GetDevice()->CreateGraphicsPipelineState(&m_CurrentPSO, IID_PPV_ARGS(&PSO));
	PSO->SetName(L"test");
	m_CommandList->SetPipelineState(PSO);
	m_CommandList->SetGraphicsRootSignature(m_RootSignature.Get());
	m_CommandList->DrawIndexedInstanced(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}

inline void D3D12HAL::SetShaderResource(U32 Slot, EShaderType Type, ID3D11ShaderResourceView* View)
{
	//m_CommandList->SetComputeRootDescriptorTable();
	/*
	ID3D11DeviceContext * ctx = GetImmediateDeviceContext();
	switch (Type)
	{
	case SHADER_TYPE_VERTEX: ctx->VSSetShaderResources(Slot, 1, &View); break;
	case SHADER_TYPE_PIXEL: ctx->PSSetShaderResources(Slot, 1, &View); break;
	case SHADER_TYPE_COMPUTE: ctx->CSSetShaderResources(Slot, 1, &View); break;
	};
	*/
}

inline void D3D12HAL::SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler)
{
	/*
	ID3D11SamplerState* res;
	m_Device->CreateSamplerState(&Sampler.desc, &res);
	ID3D11DeviceContext * ctx = GetImmediateDeviceContext();
	switch (Type)
	{
	case SHADER_TYPE_VERTEX: ctx->VSSetSamplers(Slot, 1, &res); break;
	case SHADER_TYPE_PIXEL: ctx->PSSetSamplers(Slot, 1, &res); break;
	case SHADER_TYPE_COMPUTE: ctx->CSSetSamplers(Slot, 1, &res); break;
	};
	res->Release();
	*/
}

inline void D3D12HAL::SetBlendState(BlendDesc& Blend)
{
	D3D12Interop::BlendState(m_CurrentPSO.BlendState, Blend.desc);
	m_CurrentPSO.SampleMask = 0xffffffff;
	/*
	ID3D11BlendState* pBlendState;
	m_Device->CreateBlendState(&Blend.desc, &pBlendState);
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	GetImmediateDeviceContext()->OMSetBlendState(pBlendState, blend_factor, 0xffffffff);
	pBlendState->Release();
	*/
}
