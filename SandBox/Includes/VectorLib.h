#ifndef __VECTORLIB_HH__
#define __VECTORLIB_HH__

#define _USE_MATH_DEFINES
#include <math.h>

#define PI static_cast<float>(M_PI)

inline float Cos(float a) { return cosf(a); }
inline float Sin(float a) { return sinf(a); }
inline float Clamp(float a,float min,float max) { a=(a<min)?min:a; a=(a>max)?max:a; return a; }

template <typename T>
T Max(const T& a, const T& b) { return (a > b) ? a : b; }
template <typename T>
T Min(const T& a, const T& b) { return (a < b) ? a : b; }

DECL_ALIGNED(16) struct Vec4f
{
	F32	x,y,z,w;

	Vec4f() {}
	Vec4f(F32 _x,F32 _y,F32 _z,F32 _w) : x(_x), y(_y), z(_z), w(_w) {}
	inline void Set(F32 _x,F32 _y,F32 _z,F32 _w=1.f) { x=_x; y=_y; z=_z; w=_w; }
	inline Vec4f operator + (const Vec4f&);
	inline Vec4f& operator += (const Vec4f&);
	inline Vec4f operator - (const Vec4f&);
	inline Vec4f& operator -= (const Vec4f&);
	inline Vec4f operator * (const float);
	inline void	 Normalize();
};

struct Vec2f
{
	F32 x,y;
	Vec2f() {}
	Vec2f(F32 _x,F32 _y) : x(_x), y(_y) {}
	inline void Set(F32 _x,F32 _y) { x=_x; y=_y; }
};

DECL_ALIGNED(16) struct Mat4x4
{
	F32			m[4][4];
	void SetIdentity();
	void SetTranslation(float x,float y,float z) { m[0][3]=x; m[1][3]=y; m[2][3]=z; }

	Mat4x4() {}

#ifdef _PC
	Mat4x4(const XMMATRIX& _m)
	{
		struct XMMATRIXToMat4x4 {
			float	 m[4][4];
		};
		const XMMATRIXToMat4x4 * _conv = reinterpret_cast<const XMMATRIXToMat4x4 *>(_m.r);
		m[0][0]=_conv->m[0][0]; m[0][1]=_conv->m[0][1]; m[0][2]=_conv->m[0][2]; m[0][3]=_conv->m[0][3];
		m[1][0]=_conv->m[1][0]; m[1][1]=_conv->m[1][1]; m[1][2]=_conv->m[1][2]; m[1][3]=_conv->m[1][3];
		m[2][0]=_conv->m[2][0]; m[2][1]=_conv->m[2][1]; m[2][2]=_conv->m[2][2]; m[2][3]=_conv->m[2][3];
		m[3][0]=_conv->m[3][0]; m[3][1]=_conv->m[3][1]; m[3][2]=_conv->m[3][2]; m[3][3]=_conv->m[3][3];
	}
#endif
};

#include <VectorLib.inl.h>

#endif //__VECTORLIB_HH__
