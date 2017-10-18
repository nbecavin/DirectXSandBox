#ifndef __GLBUFFERS_HH__
#define __GLBUFFERS_HH__

class GLVertexBuffer : public VertexBuffer {
private:
	//IDirect3DVertexBuffer *	res;
	GLuint					res;
public:
	GLVertexBuffer() : res(0) {}
	GLuint GetRes() { return res; }
	virtual void Create(U32 _Size,U32 _Usage,void * _Datas);
	virtual bool IsInited() { return res!=0; }
	virtual void operator = (GLVertexBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags);
	virtual void Unlock();
};

class GLIndexBuffer : public IndexBuffer {
private:
	//IDirect3DIndexBuffer *	res;
	GLuint					res;
public:
	GLIndexBuffer() : res(0) {}
	GLuint GetRes() { return res; }
	virtual void Create(U32 _Size,U32 _Usage,U32 _Fmt=FMT_IDX_16,void * _Datas=NULL);
	virtual bool IsInited() { return res!=0; }
	virtual void operator = (GLIndexBuffer& _buffer)
	{
		res = _buffer.res;
	}
	virtual bool Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags);
	virtual void Unlock();
};

/*
class DXVertexDeclaration : public VertexDeclaration {
private:
	IDirect3DVertexDeclaration	*	res;
public:
	DXVertexDeclaration() : res(NULL) {}
	IDirect3DVertexDeclaration * GetRes() { return res; }
	virtual void Create(VertexElement* Decl,U32 _ShaderUID);
};

class DXVertexShader : public VertexShader
{
private:
	IDirect3DVertexShader *	res;
public:
	DXVertexShader() : res(NULL) {}
	IDirect3DVertexShader * GetRes() { return res; }
	virtual bool IsInited() { return res!=NULL; }
};

class DXPixelShader : public PixelShader
{
private:
	IDirect3DPixelShader *		res;
public:
	DXPixelShader() : res(NULL) {}
	IDirect3DPixelShader * GetRes() { return res; }
	virtual bool IsInited() { return res!=NULL; }
};

class DXGeometryShader : public GeometryShader
{
};
*/

typedef DynArray<GLVertexBuffer,2048>		GLVertexBufferDA;
typedef DynArray<GLIndexBuffer,2048>		GLIndexBufferDA;
//typedef DynArray<DXVertexDeclaration,2048>	DXVertexDeclarationDA;
//typedef DynArray<DXVertexShader,2048>		DXVertexShaderDA;
//typedef DynArray<DXPixelShader,2048>		DXPixelShaderDA;

#endif //__GLBUFFERS_HH__
