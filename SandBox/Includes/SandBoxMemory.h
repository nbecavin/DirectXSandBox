#pragma once
#ifndef __SANDBOX_MEMORY_HH__
#define	__SANDBOX_MEMORY_HH__

#ifndef __CRTDECL
	#define __CRTDECL   __CDECL
#endif

#if defined(__cplusplus)

#ifndef __PLACEMENT_NEW_INLINE
	#define __PLACEMENT_NEW_INLINE
		inline void *	__CRTDECL operator new(size_t,void *Ptr) { return Ptr; }
		inline void		__CRTDECL operator delete(void *, void *) {}
#endif /* __PLACEMENT_NEW_INLINE */

#ifndef __PLACEMENT_VEC_NEW_INLINE
	#define __PLACEMENT_VEC_NEW_INLINE
		inline void *	__CRTDECL operator new[](size_t, void *Ptr) { return (Ptr); }
		inline void		__CRTDECL operator delete[](void *, void *) {}
#endif /* __PLACEMENT_VEC_NEW_INLINE */

//
// Overload new/delete operators

void *	__CRTDECL	operator new(size_t size);
void *	__CRTDECL	operator new[](size_t size);
void	__CRTDECL	operator delete(void *ptr);
void	__CRTDECL	operator delete[](void *ptr);

void *	__CRTDECL	operator new(size_t size,size_t align);
void *	__CRTDECL	operator new(size_t size,const char *name,const char *file,S32 line);
void *	__CRTDECL	operator new(size_t size,size_t align,const char *name,const char *file,S32 line);
void *	__CRTDECL	operator new[](size_t size,const char *name,const char *file,S32 line);

//These 3 delete operators were added to avoid warning C4291 in VS2005
void	__CRTDECL	operator delete(void *ptr,const char *name,const char *file,S32 line);
void	__CRTDECL	operator delete[](void *ptr,const char *name,const char *file,S32 line);

#endif

//
// Structure alignement directives

#ifdef _PC
	#define DECL_ALIGNED(a)	__declspec(align(a))
#else
	#define DECL_ALIGNED(a)
#endif

#endif //__SANDBOX_MEMORY_HH__
