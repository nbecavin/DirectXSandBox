#ifndef __D3D12BUFFERS_HH__
#define __D3D12BUFFERS_HH__

#include <PrimitiveBuffers.h>

class D3D12Buffer : public Buffer
{
public:
	D3D12Buffer(U32 _Size, Buffer::ECpuAccess _CpuAccess) : Buffer(_Size, _CpuAccess) {}
	ID3D12Resource* res;

	void* Map(U64 Offset);
	void Unmap();
};

class D3D12ShaderResourceView : public ShaderResourceView
{
public:
	D3D12ShaderResourceView() {}
	ID3D12Resource* res;
	D3D12_CPU_DESCRIPTOR_HANDLE CpuView;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuView;
};

class D3D12AccelerationStructure : public AccelerationStructure
{
public:
	D3D12AccelerationStructure(U32 _Size) : AccelerationStructure(_Size) {}
	ID3D12Resource* res;
	D3D12_CPU_DESCRIPTOR_HANDLE CpuView;
	D3D12_GPU_DESCRIPTOR_HANDLE GpuView;
};

class D3D12VertexBuffer : public VertexBuffer {
private:
	ID3D12Resource*				res;
	D3D12_RANGE					m_LockedRange;
	D3D12_VERTEX_BUFFER_VIEW	m_BufferView;
public:
	D3D12VertexBuffer() {}
	D3D12_VERTEX_BUFFER_VIEW GetView() { return m_BufferView; }
	virtual void Create(U32 _Size, U32 _Stride, U32 _Usage, U32 _Fmt = 0, void * _Datas = nullptr);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D12VertexBuffer& _buffer)
	{
		res = _buffer.res;
		m_BufferView = _buffer.m_BufferView;
		m_LockedRange = _buffer.m_LockedRange;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags = WriteNoOverwrite);
	virtual void Unlock();
};

class D3D12IndexBuffer : public IndexBuffer {
private:
	ID3D12Resource*				res;
	D3D12_RANGE					m_LockedRange;
	D3D12_INDEX_BUFFER_VIEW		m_BufferView;
public:
	D3D12IndexBuffer() {}
	D3D12_INDEX_BUFFER_VIEW GetView() { return m_BufferView; }
	virtual void Create(U32 _Size, U32 _Stride, U32 _Usage, U32 _Fmt = FMT_IDX_16, void * _Datas = nullptr);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D12IndexBuffer& _buffer)
	{
		res = _buffer.res; 
		m_BufferView = _buffer.m_BufferView;
		m_LockedRange = _buffer.m_LockedRange;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags = WriteNoOverwrite);
	virtual void Unlock();
};

class D3D12VertexDeclaration : public VertexDeclaration {
private:
	D3D12_INPUT_LAYOUT_DESC res;
	D3D12_INPUT_ELEMENT_DESC pElts[32];
public:
	D3D12VertexDeclaration() {}
	D3D12_INPUT_LAYOUT_DESC * GetRes() { return &res; }
	virtual void Create(VertexElement* Decl);
};

typedef DynArray<D3D12VertexDeclaration*, 8192>	D3D12VertexDeclarationDA;

#endif //__D3D12BUFFERS_HH__
