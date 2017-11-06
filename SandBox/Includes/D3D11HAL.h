#pragma once

#include <D3D11HALBuffers.h>

using namespace Microsoft::WRL;

class D3D11HAL
{
public:
	void Init(int sizeX, int sizeY, sys::Renderer* owner);
	void Shut();

	void PresentFrame();

	void CreateShaderResource(ID3DBlob * pCode, UINT Type, UINT SID);

	void InitShaders();
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
};
