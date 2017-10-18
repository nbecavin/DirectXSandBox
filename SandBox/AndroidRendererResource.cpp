#if defined(_ANDROID)

#include <AndroidRenderer.h>

using namespace sys;

VertexBuffer * AndroidRenderer::CreateVertexBuffer(U32 _Size,U32 _Usage,void * _Datas)
{
	GLVertexBuffer * buffer = m_VertexBufferDA.Add();
	buffer->Create(_Size,_Usage,_Datas);
	return (buffer->IsInited()) ? buffer : NULL;
}

IndexBuffer * AndroidRenderer::CreateIndexBuffer(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas)
{
	GLIndexBuffer * buffer = m_IndexBufferDA.Add();
	buffer->Create(_Size,_Usage,_Fmt,_Datas);
	return (buffer->IsInited()) ? buffer : NULL;
}

VertexDeclaration * AndroidRenderer::CreateVertexDecl(VertexElement* Decl,U32 _ShaderUID)
{
	MESSAGE("N/A : AndroidRenderer::CreateVertexDecl");
}

void AndroidRenderer::CreateTexture(Bitmap * _Bmap)
{
	MESSAGE("N/A : AndroidRenderer::CreateTexture");
}

static char test_vert[] = "\
		void main(){\
		}\
		";

static char test_frag[] = "\
		void main(){\
		}\
		";

void AndroidRenderer::RegisterShaderFromSourceFile(U32 _ShaderUID,const char* src,const char* epoint)
{
	MESSAGE("N/A : AndroidRenderer::RegisterShaderFromSourceFile");

	S32 profile;
	U32 Type = (_ShaderUID&SHADER_TYPE_MASK)>>SHADER_TYPE_BITS;
	U32 SID = (_ShaderUID&(~SHADER_TYPE_MASK));

	if(Type==SHADER_TYPE_VERTEX)
	{
		profile = GL_VERTEX_SHADER;
	}
	if(Type==SHADER_TYPE_PIXEL)
	{
		profile = GL_FRAGMENT_SHADER;
	}

	GLuint shader = glCreateShader(profile);

	char * pCode = NULL;
	int	   sCode = 0;
	{
		if(Type==SHADER_TYPE_VERTEX)
		{
			pCode = test_vert;
		}
		if(Type==SHADER_TYPE_PIXEL)
		{
			pCode = test_frag;
		}
		sCode = strlen(pCode);
		glShaderSource(shader,1,(const char**)&pCode,&sCode);
	}

	glCompileShader(shader);
	GLint compilestatus;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compilestatus);
	if(compilestatus!=TRUE)
	{
		MESSAGE("Shader compilation failed : %s##%s",src,epoint);
		int len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if(len > 0)
        {
            char *str = (char *) malloc(len * sizeof(char));
            if (str)
            {
                glGetShaderInfoLog(shader, len, NULL, str);
                MESSAGE("shader_debug: %s\n", str);
                free(str);
            }
        }
	}
}

#endif //_ANDROID
