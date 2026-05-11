#ifndef __SH_CONST_HH__
#define __SH_CONST_HH__

//#define USE_BINDLESS_MATERIALS

// C++ interop
typedef float2 Vec2f;
typedef float3 Vec3f;
typedef float4 Vec4f;
typedef float3x3 Mat3x3;
typedef float4x4 Mat4x4;
typedef uint U32;

#include "ShaderRegs.h"

struct ScreenVertexVsInput
{
	float4	pos : POSITION;
	float2	tex : TEXCOORD0;
};

struct ScreenVertexVsOutput
{
	float4	pos : SV_POSITION;
	float2	tex : TEXCOORD0;
};

//
// Mesh vertex

struct VS_Output
{
	float4 position : SV_POSITION;
	float3 world_position : WORLD_POSITION;
	float4 color	: COLOR0;
	float3x3 tbn	: TBN;
	float2 uv		: TEXCOORD0;
};

//
// Lighting context

cbuffer	cb_dlight
{
	float4 color;
	float3 dir;
};

struct InstanceParameters
{
	float4x4 worldMatrix;
	float4x4 invWorldMatrix;
	uint materialID;
};

struct InstanceID
{
	uint materialID;
	uint matricesID;
};

struct CameraParameters
{
	float4x4 viewMatrix;
	float4x4 projMatrix;
	float4x4 invProjMatrix;
	float4x4 invViewMatrix;
	float4 eyeWorld;
	float4 dummy0, dummy1, dummy2;
};

// Fixed CBV mapping

ConstantBuffer<GlobalParameters> Global : register(b0);
ConstantBuffer<InstanceParameters> Object : register(b1);
ConstantBuffer<InstanceID> Instance : register(b2);
ConstantBuffer<CameraParameters> Camera : register(b9);

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

float2 UVToNDC(float2 uv)
{
	return uv * float2(2.0, -2.0) + float2(-1.0, 1.0);
}

float2 NDCToUV(float2 ndc)
{
	return ndc * float2(0.5, -0.5) + float2(0.5, 0.5);
}

//
// Bindless resource access

template<typename T> T GetBindlessResource(uint index) { return (T)ResourceDescriptorHeap[NonUniformResourceIndex(index)]; }
template<typename T> T GetBindlessResourceUniform(uint index) { return (T)ResourceDescriptorHeap[index]; }

Texture2D GetTexture2D(uint index) { return GetBindlessResource<Texture2D>(index); }

#endif //__SH_CONST_HH__
