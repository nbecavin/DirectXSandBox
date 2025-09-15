#include <Renderer.h>
#include <DXRenderer.h>
#include <D3D12HAL.h>
#include <WinMain.h>

void D3D12HAL::InitShaders()
{
	HRESULT hr;
}

ShaderKernel* D3D12HAL::CreateShaderResource(ID3DBlob * pCode)
{
	ShaderKernel* aKernel = new ShaderKernel();
	D3D12Shader* aShader = new D3D12Shader();
	aKernel->m_ShaderBlob = aShader;	
	aShader->m_Blob = pCode;
	aShader->m_ByteCode.pShaderBytecode = pCode->GetBufferPointer();
	aShader->m_ByteCode.BytecodeLength = pCode->GetBufferSize();
	return aKernel;
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
	D3D12_PRIMITIVE_TOPOLOGY _primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	switch (Topology) {
	case PRIM_TRIANGLESTRIP:
		_primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		break;
	case PRIM_TRIANGLELIST:
	default:
		_primType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		break;
	}
	m_CurrentGraphicsPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	m_CommandList->IASetPrimitiveTopology(_primType);
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

VertexDeclaration* D3D12HAL::CreateVertexDecl(VertexElement *Decl)
{
	D3D12VertexDeclaration * buffer = new D3D12VertexDeclaration();
	m_InputLayoutDA.Add(buffer); //store it
	buffer->Create(Decl);
	//return (buffer->IsInited()) ? buffer : NULL;
	return buffer;
}

void D3D12HAL::SetVertexDeclaration(VertexDeclaration* Decl)
{
	D3D12VertexDeclaration* dxbuffer = reinterpret_cast<D3D12VertexDeclaration*>(Decl);
	m_CurrentGraphicsPSO.InputLayout = *dxbuffer->GetRes();
	//m_ImmediateDeviceContext->IASetInputLayout(dxbuffer->GetRes());
}

void D3D12HAL::BindGraphicPipelineState(ShaderKernel* VS, ShaderKernel* PS)
{
	m_CurrentGraphicsPSO.VS = ((D3D12Shader*)VS->m_ShaderBlob)->m_ByteCode;
	m_CurrentGraphicsPSO.PS = ((D3D12Shader*)PS->m_ShaderBlob)->m_ByteCode;
}

void D3D12HAL::BindComputePipelineState(ShaderKernel* CS)
{
	m_CurrentComputePSO.CS = ((D3D12Shader*)CS->m_ShaderBlob)->m_ByteCode;
}
