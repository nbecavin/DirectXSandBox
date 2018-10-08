#include <DXRenderer.h>
#include <D3D11HAL.h>

void D3D11HAL::SetDepthStencilState(DepthStencilDesc& Desc)
{
	ID3D11DepthStencilState* state;
	m_Device->CreateDepthStencilState(&Desc.desc, &state);
	m_ImmediateDeviceContext->OMSetDepthStencilState(state, 0);
	state->Release();
}

void D3D11HAL::SetRasterizerState(RasterizerDesc& Desc)
{
	ID3D11RasterizerState* state;
	m_Device->CreateRasterizerState(&Desc.desc, &state);
	m_ImmediateDeviceContext->RSSetState(state);
	state->Release();
}

