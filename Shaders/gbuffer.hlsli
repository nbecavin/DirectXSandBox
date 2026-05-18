#ifndef __GBUFFER_HH__
#define __GBUFFER_HH__

Texture2D<float4> GBuffer0 : register(t4);
Texture2D<float4> GBuffer1 : register(t5);
Texture2D<float4> GBuffer2 : register(t6);
Texture2D<float4> GBuffer3 : register(t7);

struct GBuffer
{
	float3 albedo;
	float3 shading_normal;
	float3 geometric_normal;
	float roughness;
	float metallic;
	float opacity;
	
	uint materialID;
	
	void Unpack(float2 uv)
	{
		uint w, h;
		GBuffer0.GetDimensions(w, h);
		int3 uvw = int3(uv.x * w, uv.y * h, 0);
		float4 slice0 = GBuffer0.Load(int3(uvw));
		float4 slice1 = GBuffer1.Load(int3(uvw));
		float4 slice2 = GBuffer2.Load(int3(uvw));
		float4 slice3 = GBuffer3.Load(int3(uvw));
		
		albedo = slice0.xyz;
		opacity = slice0.a;
		shading_normal = slice1.xyz;
		roughness = slice1.w;
		geometric_normal = slice2.xyz;
		metallic = slice2.w;
	}
};

struct PackedGBufferRT
{
	float4 GBuffer0RT : SV_TARGET0;
	float4 GBuffer1RT : SV_TARGET1;
	float4 GBuffer2RT : SV_TARGET2;
	float4 GBuffer3RT : SV_TARGET3;
	
	static PackedGBufferRT Pack(GBuffer gbuffer)
	{
		PackedGBufferRT packed;
		packed.GBuffer0RT.rgb = gbuffer.albedo;
		packed.GBuffer0RT.a = gbuffer.opacity;
		
		packed.GBuffer1RT.rgb = gbuffer.shading_normal;
		packed.GBuffer1RT.a = gbuffer.roughness;
		
		packed.GBuffer2RT.rgb = gbuffer.geometric_normal;
		packed.GBuffer2RT.a = gbuffer.metallic;
		
		packed.GBuffer3RT.r = asfloat(gbuffer.materialID);
		packed.GBuffer3RT.gba = 0;		
		return packed;
	}	
};

#endif //__GBUFFER_HH__
