#pragma once
#ifndef __SANDBOX_TYPES_HH__
#define __SANDBOX_TYPES_HH__

typedef unsigned char		U8;
typedef signed char			S8;
typedef unsigned short		U16;
typedef signed short		S16;
typedef unsigned long		U32;
typedef signed long			S32;
typedef unsigned long long	U64;
typedef signed long long	S64;
typedef float				F32;
typedef double				F64;

#if defined(_ANDROID)
typedef U8				Bool;
#else
typedef bool			Bool;
#endif

#undef TRUE
#define	TRUE	(1==1)
#undef FALSE
#define	FALSE	(0==1)

#endif //__SANDBOX_TYPES_HH__
