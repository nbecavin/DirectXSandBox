#ifndef __SH_CONST_HH__
#define __SH_CONST_HH__

#include "ShaderRegs.h"

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
	float3x3 tbn	: TBN;
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

struct CameraParameters
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 invProjMatrix;
	float4x4 invViewProjMatrix;
	float4 eyeWorld;
	float4 dummy0, dummy1, dummy2;
};

// Fixed CBV mapping

ConstantBuffer<ObjectParameters> Object : register(b1);
ConstantBuffer<CameraParameters> Camera : register(b9);

// Compatibility layer

float texDepth2D ( Texture2D depthBuffer, SamplerState depthCompare, const in float2 uv )
{
	#ifdef INVERTED_ZBUFFER
		return 1.0f - depthBuffer.Sample(depthCompare, uv ).x;
	#else
		return depthBuffer.Sample(depthCompare, uv).x;
	#endif
}

//--------------------------------------------------

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

#endif //__SH_CONST_HH__
