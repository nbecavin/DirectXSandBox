#ifndef __DX11BUFFERS_HH__
#define __DX11BUFFERS_HH__

class D3D11VertexBuffer : public VertexBuffer {
private:
	ID3D11Buffer *	res;
public:
	D3D11VertexBuffer() : res(NULL) {}
	ID3D11Buffer * GetRes() { return res; }
	virtual void Create(U32 _Size, U32 _Usage, U32 _Fmt = 0, void * _Datas = nullptr);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D11VertexBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags = WriteNoOverwrite);
	virtual void Unlock();
};

class D3D11IndexBuffer : public IndexBuffer {
private:
	ID3D11Buffer *	res;
public:
	D3D11IndexBuffer() : res(NULL) {}
	ID3D11Buffer * GetRes() { return res; }
	virtual void Create(U32 _Size, U32 _Usage, U32 _Fmt = FMT_IDX_16, void * _Datas = nullptr);
	virtual bool IsInited() { return res != NULL; }
	virtual void operator = (D3D11IndexBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock, U32 SizeToLock, void **pData, EMap Flags = WriteNoOverwrite);
	virtual void Unlock();
};

class D3D11VertexDeclaration : public VertexDeclaration {
private:
	ID3D11InputLayout *	res;
public:
	D3D11VertexDeclaration() : res(NULL) {}
	ID3D11InputLayout * GetRes() { return res; }
	virtual void Create(VertexElement* Decl, U32 _ShaderUID);
};

typedef DynArray<D3D11VertexDeclaration, 2048>	D3D11VertexDeclarationDA;

#endif //__DX11BUFFERS_HH__
