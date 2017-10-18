#include <stdlib.h>

void * __CRTDECL	operator new(size_t size)
{
#if defined(_PC)
	void* ptr = ::_aligned_malloc(size,16);//align);
	return ptr;
#else
	void* ptr = ::malloc(size);//align);
	return ptr;
#endif
}

void * __CRTDECL	operator new[](size_t size)
{
#if defined(_PC)
	void* ptr = ::_aligned_malloc(size,16);//align);
	return ptr;
#else
	void* ptr = ::malloc(size);//align);
	return ptr;
#endif
}

void __CRTDECL	operator delete(void *ptr)
{
#if defined(_PC)
	::_aligned_free(ptr);
#else
	free(ptr);
#endif
}

void __CRTDECL	operator delete[](void *ptr)
{
#if defined(_PC)
	::_aligned_free(ptr);
#else
	free(ptr);
#endif
}

void * __CRTDECL	operator new(size_t size,size_t align)
{
#if defined(_PC)
	void* ptr = ::_aligned_malloc(size,align);
	return ptr;
#else
	void* ptr = ::malloc(size);//align);
	return ptr;
#endif
}
