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


#define	DECLARE_INLINE_ENUM_FLAG_OPERATORS_Z(_enumType) \
	/*ASSERT_AT_COMPILE_TIME_Z(sizeof(_enumType) == sizeof(U32));*/ \
	inline constexpr _enumType operator+(_enumType _a, _enumType _b){ return _enumType( ((U32)_a) | ((U32)_b) ); } \
	inline constexpr _enumType& operator+=(_enumType& _a, _enumType _b){ _a = _enumType(((U32)_a) | ((U32)_b)); return _a; } \
	inline constexpr _enumType operator|(_enumType _a, _enumType _b){ return _enumType( ((U32)_a) | ((U32)_b) ); } \
	inline constexpr _enumType& operator|=(_enumType& _a, _enumType _b){ _a = _enumType(((U32)_a) | ((U32)_b)); return _a; } \
	inline constexpr _enumType operator&(_enumType _a, _enumType _b){ return _enumType((U32)_a & (U32)_b); } \
	inline constexpr _enumType& operator&=(_enumType& _a, _enumType _b){ _a = _enumType(((U32)_a) & ((U32)_b)); return _a; } \
	inline constexpr _enumType operator^(_enumType _a, _enumType _b){ return _enumType((U32)_a ^ (U32)_b); } \
	inline constexpr _enumType& operator^=(_enumType& _a, _enumType _b){ _a = _enumType(((U32)_a) ^ ((U32)_b)); return _a; } \
	inline constexpr _enumType operator~(_enumType _a){ return _enumType( ~((U32)_a) ); }\
	inline constexpr _enumType operator++(_enumType &_a) { _a = (_enumType)(((U32)_a)+1); return _a;} \
	inline constexpr _enumType operator++(_enumType &_a, int) { _enumType ret=_a; _a=(_enumType)(((U32)_a)+1); return ret; } \
	inline constexpr _enumType operator--(_enumType &_a) { _a = (_enumType)(((U32)_a)-1); return _a;} \
	inline constexpr _enumType operator--(_enumType &_a, int) { _enumType ret=_a; _a=(_enumType)(((U32)_a)-1); return ret; } \
	inline constexpr _enumType operator<<(_enumType _a,int n){ return _enumType( ((U32)_a)<<n ); } \
	inline constexpr _enumType& operator<<=(_enumType& _a,int n){ _a = _enumType(((U32)_a) << n); return _a; } \
	inline constexpr _enumType operator>>(_enumType _a,int n){ return _enumType( ((U32)_a)>>n ); } \
	inline constexpr _enumType& operator>>=(_enumType& _a,int n){ _a = _enumType(((U32)_a) >> n); return _a; }


#endif //__SANDBOX_TYPES_HH__
