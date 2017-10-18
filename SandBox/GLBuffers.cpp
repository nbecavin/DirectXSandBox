#if defined(_ANDROID)

#include <AndroidRenderer.h>

void GLVertexBuffer::Create(U32 _Size,U32 _Usage,void * _Datas)
{
	if(_Datas)
	{
		glGenBuffers(1, &res);
		glBindBuffer(GL_ARRAY_BUFFER,res);
		glBufferData(GL_ARRAY_BUFFER,_Size,_Datas,GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	else
	{
		res = (GLuint) malloc( _Size );
	}
}

void GLIndexBuffer::Create(U32 _Size,U32 _Usage,U32 _Fmt,void * _Datas)
{
	if(_Datas)
	{
		glGenBuffers(1, &res);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,res);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,_Size,_Datas,GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}
	else
	{
		res = (GLuint) malloc( _Size );
	}
}

bool GLVertexBuffer::Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags)
{
	MESSAGE("GLVertexBuffer::Lock");
	*pData = (unsigned char *)res+OffsetToLock;
}

bool GLIndexBuffer::Lock(U32 OffsetToLock,U32 SizeToLock,void **pData,U32 Flags)
{
	MESSAGE("GLIndexBuffer::Lock");
	*pData = (unsigned char *)res+OffsetToLock;
}

void GLVertexBuffer::Unlock()
{
}

void GLIndexBuffer::Unlock()
{
}

#endif //_ANDROID
