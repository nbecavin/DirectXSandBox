#ifndef __SH_CONST_HH__
#define __SH_CONST_HH__

#include "ShaderRegs.h"

const float4 c[256] : register(c0);

// old fashion constants (default to b0)
//const float4x4 gInvProjMatrix;

float4x4 GetMat4x4(in const int cst)
{
	return float4x4( c[cst+0].x, c[cst+1].x, c[cst+2].x, c[cst+3].x,
						c[cst+0].y, c[cst+1].y, c[cst+2].y, c[cst+3].y,
						c[cst+0].z, c[cst+1].z, c[cst+2].z, c[cst+3].z,
						c[cst+0].w, c[cst+1].w, c[cst+2].w, c[cst+3].w );
}

struct ScreenVertexVsInput
{
	float4	pos : POSITION;
	float2	tex : TEXCOORD0;
};

struct ScreenVertexVsOutput
{
	float4	pos : POSITION;
	float2	tex : TEXCOORD0;
};


//
// Mesh vertex

struct VS_Output
{
	float4 position : SV_POSITION;//POSITION;
	float4 color	: COLOR0;
	float3 normal	: COLOR1;
	float2 uv		: TEXCOORD0;
	float3 eyevec	: TEXCOORD1;
};

//
// Lighting context
cbuffer	cb_dlight
{
	float4	color;
	float3	dir;
};

struct ObjectParameters
{
	float4x4	worldMatrix;
	float4x4	invWorldMatrix;
};
ConstantBuffer<ObjectParameters> Object;

struct CameraParameters
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 invProjMatrix;
	float4x4 invViewProjMatrix;
};
ConstantBuffer<CameraParameters> Camera : register(b9);

// Compatibility layer

float	texDepth2D ( Texture2D depthBuffer, SamplerState depthCompare, const in float2 uv )
{
	#ifdef INVERTED_ZBUFFER
		return 1.0f - tex2D( depthBuffer, uv ).x;
	#else
		return depthBuffer.Sample(depthCompare, uv).x;
	#endif
}

//--------------------------------------------------

// Matrix ordering is different from PS3 and DX/360
// You MUST use GetMatrix() to get matrix from constants
// You MUST use Mul (instead of mul) to use this matrix
// See Wiki : http://192.168.0.12/mediawiki/index.php/Shaders#Shaders_Development_Guidelines
// Matrices are column major
#if defined(VP)
float4x4 GetMatrix(int CST)
{
	return float4x4( c[CST].x,c[CST+1].x,c[CST+2].x,c[CST+3].x,
						c[CST].y,c[CST+1].y,c[CST+2].y,c[CST+3].y,
						c[CST].z,c[CST+1].z,c[CST+2].z,c[CST+3].z,
						c[CST].w,c[CST+1].w,c[CST+2].w,c[CST+3].w );
}
#endif
float3x3 SetMatrix3x3( float3 u, float3 v, float3 w )
{
	return float3x3( u.x, v.x, w.x,
						u.y, v.y, w.y,
						u.z, v.z, w.z );
}
float4x4 SetMatrix4x4( float4 u, float4 v, float4 w, float4 i )
{
	return float4x4( u.x, v.x, w.x, i.x,
						u.y, v.y, w.y, i.y,
						u.z, v.z, w.z, i.z,
						u.w, v.w, w.w, i.w );
}
#define	Mul(v,m)	mul(v,m)

#endif //__SH_CONST_HH__
