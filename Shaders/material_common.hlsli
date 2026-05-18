#ifndef __MATERIAL_COMMON_HH__
#define __MATERIAL_COMMON_HH__

#include "brdf.hlsli"
#include "material_parameter.hlsli.h"

// -------------------------------
// Physically Based Materials
// -------------------------------

SamplerState sSampler : register(s0);
#define LinearSampler sSampler

#ifdef USE_BINDLESS_MATERIALS
MaterialParameter GetBindlessMaterial(uint index)
{
	StructuredBuffer<MaterialParameter> store = GetBindlessResource(0);
	return store[index];
}
#else
StructuredBuffer<MaterialParameter> MaterialStore : register(t16);
MaterialParameter GetMaterial(uint index)
{
	return MaterialStore[index];
}
#endif

struct MaterialPBR
{
	float3 albedo;
	float3 normal;
	float roughness;
	float metallic;
	float opacity;
	float ao;
	float3 emission;
};

MaterialPBR SampleMaterial(in VS_Output i)
{
	MaterialParameter p = GetMaterial(Instance.materialID);
	
	MaterialPBR mat = (MaterialPBR) 0;

	float4 albedo_and_opacity = GetTexture2D(p.albedo_map).Sample(LinearSampler, i.uv);

    // If your albedo texture is stored in sRGB and sampled as linear by sampler state, you can skip SRGBToLinear.
    // If you get sRGB values unconverted, convert:
    // albedo = SRGBToLinear(albedo);

	float roughness = GetTexture2D(p.roughness_map).Sample(LinearSampler, i.uv).r * p.roughness;
    /*float3 mra = g_MRATexture.Sample(LinearSampler, IN.uv).rgb;
    float metallic = mra.r;
    float roughness = mra.g;
    float ao = mra.b;*/

	float3 emissive = p.emission; //g_EmissiveTex.Sample(LinearSampler, IN.uv).rgb;

    // Normal mapping
	float3 normal = GetTexture2D(p.normal_map).Sample(sSampler, i.uv).xyz; // * 2 - 1;
	//normal = float3(0, 0, 1);
	//normal = normalize(mul(i.tbn, normal)); //tangent space to local space

	normal = normalize(mul(normal, i.tbn)); //tangent space to local space

	mat.albedo = albedo_and_opacity.xyz * p.albedo;
	mat.opacity = albedo_and_opacity.w * p.opacity;
	mat.normal = normal;
	mat.metallic = p.metallic;
	mat.roughness = clamp(roughness, 0.04, 1.0); // avoid 0.0 roughness which can alias
	mat.ao = saturate(1);
	mat.emission = emissive;

	return mat;
}

// Evaluate PBR lighting given material, geometry and view
float3 EvaluateBRDF(MaterialPBR m, float3 LightColor, float3 L, float3 N, float3 V)
{
	float3 Vdir = normalize(V); // view direction (world space) pointing from surface to camera
	float3 Ldir = normalize(L); // assuming directional light direction stored as "light forward"
	float3 H = normalize(Vdir + Ldir);

    // Calculate base reflectivity F0 (dielectric vs metallic)
	float3 F0 = GetSpecularF0(m.albedo, m.metallic); // metals use albedo as F0

    // NdotV, NdotL etc.
	float NdotV = max(dot(N, Vdir), 0.0001);
	float NdotL = max(dot(N, Ldir), 0.0);
	float NdotH = max(dot(N, H), 0.0);
	float VdotH = max(dot(Vdir, H), 0.0);

    // Cook-Torrance BRDF
	float3 F = FresnelSchlick(VdotH, F0);
	float D = GGX_NDF(NdotH, m.roughness);
	float Vis = V_SmithJointApprox(NdotV, NdotL, m.roughness);
	float3 specular = F * D * Vis;

    // Lambertian diffuse (albedo / PI)
	float3 diffuse = GetDiffuseReflectance(m.albedo, m.metallic) * (1.f - F) / PI;

    // Direct lighting (single directional light)
	float3 radiance = LightColor; // incoming radiance from light
	float3 Lo = (diffuse + specular) * radiance * NdotL;

    // Image-based lighting (IBL) for ambient/specular
    // Diffuse IBL: use irradiance map (low-frequency)
//	float3 irradiance = g_EnvIrradiance.Sample(LinearSampler, /* direction -> sample method depends on representation */N).rgb;
//	float3 diffuseIBL = irradiance * m.albedo;

    // Specular IBL: sample prefiltered specular environment map + BRDF LUT
//	float3 R = reflect(-Vdir, N);
//	float3 prefilteredColor = PrefilteredEnvSample(R, m.roughness);
//	float2 brdf = IntegrateBRDF(NdotV, m.roughness).xy; // returns scale and bias typically
    // Combine: prefiltered * (F * brdf.x + brdf.y)
//	float3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    // Ambient occlusion modulate IBL
	//float3 ambient = (diffuseIBL * (1.0 - kS) + specularIBL) * m.ao;
	float3 ambient = 0;

    // Final color
	float3 color = Lo + ambient + m.emission;
	return color;
}

#endif //__MATERIAL_COMMON_HH__
