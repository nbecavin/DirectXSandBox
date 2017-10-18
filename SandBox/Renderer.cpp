#include <Renderer.h>

namespace sys {

	int Renderer::Init()
	{
		return 0;
	}

	VertexBuffer*	Renderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas)
	{
		return NULL;
	}

	IndexBuffer*	Renderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas)
	{
		return NULL;
	}

	VertexDeclaration*	Renderer::CreateVertexDecl(VertexElement * _Decl,U32 _ShaderUID)
	{
		return NULL;
	}

};
