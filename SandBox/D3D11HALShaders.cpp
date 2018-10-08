#include "stdafx.h"
#if defined(_PCDX11)
#include <Renderer.h>
#include <D3D11HAL.h>
#include <D3D11HALBuffers.h>
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

void D3D11HAL::SetIndices(IndexBuffer* Buffer, U32 _Fmt)
{
	DXIndexBuffer* dxbuffer = reinterpret_cast<DXIndexBuffer*>(Buffer);
	m_ImmediateDeviceContext->IASetIndexBuffer(dxbuffer->GetRes(), (_Fmt == FMT_IDX_32) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
}

void D3D11HAL::SetPrimitiveTopology(PrimitiveType Topology)
{
	int index_count = 0;
	D3D11_PRIMITIVE_TOPOLOGY _primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (Topology) {
	case PRIM_TRIANGLESTRIP:
		_primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case PRIM_TRIANGLELIST:
	default:
		_primType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
	m_ImmediateDeviceContext->IASetPrimitiveTopology(_primType);
}

void D3D11HAL::SetStreamSource(U32 StreamNumber, VertexBuffer* Buffer, U32 Offset, U32 Stride)
{
	DXVertexBuffer* dxbuffer = reinterpret_cast<DXVertexBuffer*>(Buffer);
	ID3D11Buffer * pBuffer = dxbuffer->GetRes();
	UINT _Stride = Stride;
	UINT _Offset = Offset;
	m_ImmediateDeviceContext->IASetVertexBuffers(StreamNumber, 1, &pBuffer, &_Stride, &_Offset);
}

VertexDeclaration* D3D11HAL::CreateVertexDecl(VertexElement *Decl, U32 _ShaderUID)
{
	D3D11VertexDeclaration * buffer = m_InputLayoutDA.Add();
	MESSAGE("Begin create Decl");
	buffer->Create(Decl, _ShaderUID);
	//return (buffer->IsInited()) ? buffer : NULL;
	MESSAGE("End create Decl");
	return buffer;
}

void D3D11HAL::SetVertexDeclaration(VertexDeclaration* Decl)
{
	D3D11VertexDeclaration* dxbuffer = reinterpret_cast<D3D11VertexDeclaration*>(Decl);
	m_ImmediateDeviceContext->IASetInputLayout(dxbuffer->GetRes());
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
