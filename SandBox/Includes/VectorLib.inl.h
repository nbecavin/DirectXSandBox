	
//
// VectorLib.h implementation

inline void Mat4x4::SetIdentity()
{
	m[0][0]=1.f; m[0][1]=0.f; m[0][2]=0.f; m[0][3]=0.f;
	m[1][0]=0.f; m[1][1]=1.f; m[1][2]=0.f; m[1][3]=0.f;
	m[2][0]=0.f; m[2][1]=0.f; m[2][2]=1.f; m[2][3]=0.f;
	m[3][0]=0.f; m[3][1]=0.f; m[3][2]=0.f; m[3][3]=1.f;
}

namespace Vec4
{
	static inline void Dot4(Vec4f* res,const Vec4f* v0,const Vec4f* v1)
	{
		Vec4f v;
		v.x = v0->x*v1->x + v0->y*v1->y + v0->z*v1->z + v0->w*v1->w;
		v.w = v.z = v.y = v.x;
		*res = v;
	}
	static inline void Dot3(Vec4f* res,const Vec4f* v0,const Vec4f* v1)
	{
		Vec4f v;
		v.x = v0->x*v1->x + v0->y*v1->y + v0->z*v1->z;
		v.w = v.z = v.y = v.x;
		*res = v;
	}
	static inline void Cross(Vec4f* res,const Vec4f* v0,const Vec4f* v1)
	{
		Vec4f v;
		v.x = v0->y * v1->z - v0->z * v1->y;
		v.y = v0->z * v1->x - v0->x * v1->z;
		v.z = v0->x * v1->y - v0->y * v1->x;
		*res = v;
	}
	static inline void Normalize(Vec4f* res,const Vec4f* v)
	{
		Vec4f norm;
		Dot3(&norm,v,v);
		F32 invNorm = 1.f/sqrtf(norm.x);
		Vec4f r;
		r.x = v->x*invNorm;
		r.y = v->y*invNorm;
		r.z = v->z*invNorm;
		r.w = 1.f;
		*res = r;
	}
};

//
// Class methods

inline Vec4f Vec4f::operator + (const Vec4f& _v)
{
	return Vec4f( this->x + _v.x,
				  this->y + _v.y,
				  this->z + _v.z,
				  this->w + _v.w );
}
inline Vec4f& Vec4f::operator += (const Vec4f& _v)
{
	*this = *this + _v;
	return *this;
}

inline Vec4f Vec4f::operator - (const Vec4f& _v)
{
	return Vec4f( this->x - _v.x,
				  this->y - _v.y,
				  this->z - _v.z,
				  this->w - _v.w );
}
inline Vec4f& Vec4f::operator -= (const Vec4f& _v)
{
	*this = *this - _v;
	return *this;
}

inline Vec4f Vec4f::operator * (const float _f)
{
	return Vec4f( this->x * _f,
				  this->y * _f,
				  this->z * _f,
				  this->w * _f );
}

inline void Vec4f::Normalize()
{
	Vec4::Normalize(this,this);
}
