#pragma once

#include <D3D11HALBuffers.h>

using namespace Microsoft::WRL;

class D3D11HAL
{
public:
	void Init(int sizeX, int sizeY, sys::Renderer* owner);
	void Shut();

	void PresentFrame();

	VertexDeclaration *	CreateVertexDecl(VertexElement* Decl, U32 _ShaderUID);
	void CreateShaderResource(ID3DBlob * pCode, UINT Type, UINT SID);
	void CreateTexture(Bitmap * _Bm);

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

	ID3D11Device* GetDevice() { return m_Device.Get(); }
	ID3D11DeviceContext* GetImmediateDeviceContext() { return m_ImmediateDeviceContext.Get(); }

	// Graphics command list
	void SetAndClearRenderTarget();
	void SetDepthStencilState(DepthStencilDesc& Desc);
	void SetRasterizerState(RasterizerDesc& Desc);
	inline void SetBlendState(BlendDesc& desc);
	inline void SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler);
	inline void SetShaderResource(U32 Slot, EShaderType Type, ID3D11ShaderResourceView* View);
	inline void DrawIndexed(UINT IndexCount,UINT StartIndexLocation,INT BaseVertexLocation);
	inline void SetViewports(D3D11_VIEWPORT& Viewport);

	// --- to remove
	IDXGISwapChain* GetSwapChain() { return m_SwapChain.Get(); }
	// --- to remove

private:
	ComPtr<IDXGIFactory1>				m_DxgiFactory;
	ComPtr<IDXGISwapChain>				m_SwapChain;
	ComPtr<ID3D11Device>				m_Device;
	ComPtr<ID3D11DeviceContext>			m_ImmediateDeviceContext;

	ID3D11VertexShader *			m_VSDA[SHADER_VS_COUNT];
	ID3D11PixelShader *				m_PSDA[SHADER_PS_COUNT];
	ID3D11ComputeShader *			m_CSDA[SHADER_CS_COUNT];
	ID3DBlob *						m_VSDABlob[SHADER_VS_COUNT];
	ID3DBlob *						m_PSDABlob[SHADER_PS_COUNT];
	ID3DBlob *						m_CSDABlob[SHADER_CS_COUNT];
	ID3D11ShaderReflection *		m_VSDAReflection[SHADER_VS_COUNT];
	ID3D11ShaderReflection *		m_PSDAReflection[SHADER_PS_COUNT];
	ID3D11ShaderReflection *		m_CSDAReflection[SHADER_CS_COUNT];

	D3D11VertexDeclarationDA		m_InputLayoutDA;

	// Surfaces
	ID3D11RenderTargetView *		m_BackBuffer;
	ID3D11DepthStencilView *		m_DepthBuffer;
	ID3D11ShaderResourceView *		m_ZBuffer;

#define VS_CONSTANT_MAX_COUNT	256
#define VS_CONSTANT_BUFFER_SIZE	(VS_CONSTANT_MAX_COUNT*sizeof(Vec4f))
	ID3D11Buffer *					m_VSConstant;

	ID3D11Buffer *					m_SHHemisphere;

	ID3D11Buffer *					m_CameraConstant;
	ID3D11Buffer *					m_PostProcessConstant;
};

inline void D3D11HAL::SetViewports(D3D11_VIEWPORT& Vp)
{
	m_ImmediateDeviceContext->RSSetViewports(1, &Vp);
}

inline void D3D11HAL::DrawIndexed(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	GetImmediateDeviceContext()->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

inline void D3D11HAL::SetShaderResource(U32 Slot, EShaderType Type, ID3D11ShaderResourceView* View)
{
	ID3D11DeviceContext * ctx = GetImmediateDeviceContext();
	switch (Type)
	{
	case SHADER_TYPE_VERTEX: ctx->VSSetShaderResources(Slot, 1, &View); break;
	case SHADER_TYPE_PIXEL: ctx->PSSetShaderResources(Slot, 1, &View); break;
	case SHADER_TYPE_COMPUTE: ctx->CSSetShaderResources(Slot, 1, &View); break;
	};
}

inline void D3D11HAL::SetSampler(U32 Slot, EShaderType Type, SamplerDesc& Sampler)
{
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
}

inline void D3D11HAL::SetBlendState(BlendDesc& Blend)
{
	ID3D11BlendState* pBlendState;
	m_Device->CreateBlendState(&Blend.desc, &pBlendState);
	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	GetImmediateDeviceContext()->OMSetBlendState(pBlendState, blend_factor, 0xffffffff);
	pBlendState->Release();
}
