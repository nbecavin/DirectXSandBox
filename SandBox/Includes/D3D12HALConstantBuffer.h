#pragma once
#ifndef __D3D12CONSTANTBUFFER_HH__
#define __D3D12CONSTANTBUFFER_HH__

// Implement a dynamic constant buffer using a simple ring buffer strategy
class D3D12ConstantBuffer : public ConstantBuffer
{
public:
	D3D12ConstantBuffer() : res(NULL) {}
	ID3D12Resource * GetRes() { return res; }
	virtual void Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas) {}
	virtual void Create(U32 _Size);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D12ConstantBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap MapFlags = WriteNoOverwrite);
	virtual void Unlock();

	struct RingBuffer
	{
		D3D12_GPU_VIRTUAL_ADDRESS m_BeginPtr;
		D3D12_GPU_VIRTUAL_ADDRESS m_WritePtr;
		D3D12_GPU_VIRTUAL_ADDRESS m_EndPtr;
	};

protected:
	RingBuffer	m_RingConfig;
	ID3D12Resource* res;
	D3D12_RANGE					m_LockedRange;
	D3D12_CONSTANT_BUFFER_VIEW_DESC		m_BufferView;
};

#endif //__D3D12CONSTANTBUFFER_HH__

