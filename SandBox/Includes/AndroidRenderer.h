#ifndef __ANDROID_RENDERER_HH__
#define __ANDROID_RENDERER_HH__

#include <Renderer.h>
#include <ShaderConstants.h>
#include <GLES2/gl2.h>
#include <GLBuffers.h>

struct Vertex2D
{
	float		x,y,z;
	float		u,v;
};

namespace sys {

	class AndroidRenderer : public Renderer
	{
	public:
		virtual int		Init();
		virtual bool	InitStaticDatas();
		virtual void	InitShaders();
				Bool	Resume();
				void	Pause();
		virtual void	Shut();
		virtual void	ReleaseAllResources();
		virtual void	MainLoop();

		virtual VertexBuffer *		CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas);
		virtual IndexBuffer *		CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas);
		virtual VertexDeclaration *	CreateVertexDecl(VertexElement* Decl,U32 _ShaderUID);
		virtual void				CreateTexture(Bitmap * _Bmap);

		virtual void	RegisterShaderFromSourceFile(U32 _SUID,const char* src,const char* epoint);

	private:
		GLVertexBufferDA				m_VertexBufferDA;
		GLIndexBufferDA					m_IndexBufferDA;
		//GLVertexDeclarationDA			m_VertexDeclarationDA;
		//GLVertexShaderDA				m_VertexShaderDA;
		//GLPixelShaderDA					m_PixelShaderDA;

	};

};

#endif //__ANDROID_RENDERER_HH__
