#include "stdafx.h"
#if defined(_PCDX11)
#include <Renderer.h>
#include <D3D11HAL.h>
#include <WinMain.h>

void D3D11HAL::InitShaders()
{
	HRESULT hr;

	memset(m_VSDA, 0, sizeof(m_VSDA));
	memset(m_PSDA, 0, sizeof(m_PSDA));
	memset(m_VSDABlob, 0, sizeof(m_VSDABlob));
	memset(m_PSDABlob, 0, sizeof(m_PSDABlob));
}

void D3D11HAL::CreateShaderResource(ID3DBlob * pCode, UINT Type, UINT SID)
{
	HRESULT hr;
	if (Type == SHADER_TYPE_VERTEX)
	{
		m_VSDABlob[SID] = pCode;

		hr = GetDevice()->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_VSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create pixel shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_VSDAReflection[SID]);
		}
	}
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_PSDABlob[SID] = pCode;

		hr = GetDevice()->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_PSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create pixel shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_PSDAReflection[SID]);
		}
	}
	if (Type == SHADER_TYPE_COMPUTE)
	{
		m_CSDABlob[SID] = pCode;

		hr = GetDevice()->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_CSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create compute shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_CSDAReflection[SID]);
		}
	}
}

void D3D11HAL::SetShaders(UINT Type, UINT SID)
{
	if (Type == SHADER_TYPE_VERTEX)
	{
		m_ImmediateDeviceContext->VSSetShader(m_VSDA[SID], NULL, 0);
	}
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_ImmediateDeviceContext->PSSetShader(m_PSDA[SID], NULL, 0);
	}
}

#endif
