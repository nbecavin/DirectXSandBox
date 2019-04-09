#pragma once
#ifndef __D3D11CONSTANTBUFFER_HH__
#define __D3D11CONSTANTBUFFER_HH__

class D3D11ConstantBuffer : public ConstantBuffer
{
private:
	ID3D11Buffer *	res;
public:
	D3D11ConstantBuffer() : res(NULL) {}
	ID3D11Buffer * GetRes() { return res; }
	virtual void Create(U32 _Size, U32 _Usage, U32 _Fmt, void * _Datas) {}
	virtual void Create(U32 _Size);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D11ConstantBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap MapFlags = WriteNoOverwrite);
	virtual void Unlock();
};

#endif //__D3D12CONSTANTBUFFER_HH__

