#pragma once
#ifndef __D3D12CONSTANTBUFFER_HH__
#define __D3D12CONSTANTBUFFER_HH__

// Implement a dynamic constant buffer using a simple ring buffer strategy
class D3D12ConstantBuffer
{
public:
	void Init();

	struct RingBuffer
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_BeginPtr;
		D3D12_GPU_VIRTUAL_ADDRESS m_WritePtr;
		D3D12_GPU_VIRTUAL_ADDRESS m_EndPtr;
	};

protected:
	RingBuffer	m_RingConfig;
	ID3D12Resource* m_CB;
};

#endif //__D3D12CONSTANTBUFFER_HH__

