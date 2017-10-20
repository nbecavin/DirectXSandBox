#include "stdafx.h"
#if defined(_PCDX12)
#include <Renderer.h>
#include <D3D12HAL.h>
#include <WinMain.h>

void D3D12HAL::InitShaders()
{
	HRESULT hr;
	memset(m_VSDABlob, 0, sizeof(m_VSDABlob));
	memset(m_PSDABlob, 0, sizeof(m_PSDABlob));
}

void D3D12HAL::CreateShaderResource(ID3DBlob * pCode, UINT Type, UINT SID)
{
	if (Type == SHADER_TYPE_VERTEX)
	{
		m_VSDABlob[SID] = pCode;

		m_VSDA[SID].pShaderBytecode = pCode->GetBufferPointer();
		m_VSDA[SID].BytecodeLength = pCode->GetBufferSize();

#ifdef _PCDX11
		hr = GetDevice()->CreateVertexShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_VSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create pixel shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_VSDAReflection[SID]);
		}
#endif
	}
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_PSDABlob[SID] = pCode;

		m_PSDA[SID].pShaderBytecode = pCode->GetBufferPointer();
		m_PSDA[SID].BytecodeLength = pCode->GetBufferSize();

#ifdef _PCDX11
		hr = GetDevice()->CreatePixelShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_PSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create pixel shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_PSDAReflection[SID]);
		}
#endif
	}
	if (Type == SHADER_TYPE_COMPUTE)
	{
		m_CSDABlob[SID] = pCode;

		m_CSDA[SID].pShaderBytecode = pCode->GetBufferPointer();
		m_CSDA[SID].BytecodeLength = pCode->GetBufferSize();

#ifdef _PCDX11
		hr = GetDevice()->CreateComputeShader(pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_CSDA[SID]);
		if (hr != S_OK)
			MessageBox(NULL, "Cannot create compute shader", "Shader Error", MB_OK);
		else
		{
			D3DReflect(pCode->GetBufferPointer(), pCode->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&m_CSDAReflection[SID]);
		}
#endif
	}
}

void D3D12HAL::SetShaders(UINT Type, UINT SID)
{
	if (Type == SHADER_TYPE_VERTEX)
	{
		m_CurrentPSO.VS = m_VSDA[SID];
	}
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_CurrentPSO.PS = m_PSDA[SID];
	}
}

#endif
