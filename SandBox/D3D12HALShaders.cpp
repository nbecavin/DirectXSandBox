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
	}
	if (Type == SHADER_TYPE_PIXEL)
	{
		m_PSDABlob[SID] = pCode;

		m_PSDA[SID].pShaderBytecode = pCode->GetBufferPointer();
		m_PSDA[SID].BytecodeLength = pCode->GetBufferSize();
	}
	if (Type == SHADER_TYPE_COMPUTE)
	{
		m_CSDABlob[SID] = pCode;

		m_CSDA[SID].pShaderBytecode = pCode->GetBufferPointer();
		m_CSDA[SID].BytecodeLength = pCode->GetBufferSize();
	}
}

void D3D12HAL::SetIndices(IndexBuffer* Buffer, U32 _Fmt)
{
	D3D12IndexBuffer* dxbuffer = reinterpret_cast<D3D12IndexBuffer*>(Buffer);
	//m_ImmediateDeviceContext->IASetIndexBuffer(dxbuffer->GetRes(), (_Fmt == FMT_IDX_32) ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, 0);
	GetCommandList()->IASetIndexBuffer(&dxbuffer->GetView());
}

void D3D12HAL::SetPrimitiveTopology(PrimitiveType Topology)
{
	int index_count = 0;
	D3D_PRIMITIVE_TOPOLOGY _primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (Topology) {
	case PRIM_TRIANGLESTRIP:
		_primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case PRIM_TRIANGLELIST:
	default:
		_primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
	//m_ImmediateDeviceContext->IASetPrimitiveTopology(_primType);
}

void D3D12HAL::SetStreamSource(U32 StreamNumber, VertexBuffer* Buffer, U32 Offset, U32 Stride)
{
	D3D12VertexBuffer* dxbuffer = reinterpret_cast<D3D12VertexBuffer*>(Buffer);

	D3D12_VERTEX_BUFFER_VIEW view = dxbuffer->GetView();
	view.BufferLocation += Offset * Stride;
	view.StrideInBytes = Stride;
	GetCommandList()->IASetVertexBuffers(StreamNumber, 1, &view);

	//m_StateCache.VB = dxbuffer;
}

VertexDeclaration* D3D12HAL::CreateVertexDecl(VertexElement *Decl, U32 _ShaderUID)
{
	D3D12VertexDeclaration * buffer = m_InputLayoutDA.Add();
	MESSAGE("Begin create Decl");
	buffer->Create(Decl, _ShaderUID);
	//return (buffer->IsInited()) ? buffer : NULL;
	MESSAGE("End create Decl");
	return buffer;
}

void D3D12HAL::SetVertexDeclaration(VertexDeclaration* Decl)
{
	D3D12VertexDeclaration* dxbuffer = reinterpret_cast<D3D12VertexDeclaration*>(Decl);
	//m_ImmediateDeviceContext->IASetInputLayout(dxbuffer->GetRes());
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
