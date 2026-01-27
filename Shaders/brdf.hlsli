#pragma once

// BRDF: GGX / Smith-Schlick
// -------------------------------

#define GGX_MIN_ROUGHNESS 0.01

float ClampRoughness(float r)
{
	return max(r, GGX_MIN_ROUGHNESS);
}

// GGX / Trowbridge-Reitz NDF
float GGX_NDF(float NdotH, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float denom = (NdotH * NdotH) * (a2 - 1.0) + 1.0;
	denom = PI * denom * denom;
	return a2 / max(denom, 1e-6);
}

// Schlick-GGX Geometry term (single direction)
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0; // UE4 optimization, energy-preserving
	return NdotV / (NdotV * (1.0 - k) + k);
}

// Smith’s method: combine view & light terms
float G_Smith(float NdotV, float NdotL, float roughness)
{
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
}

float V_Smith(float NdotV, float NdotL, float roughness)
{
	float G = G_Smith(NdotV, NdotL, roughness);
	return G / (4.0 * NdotV * NdotL + 1e-6);
}

// This is the "Joint" version which correlates the masking and shadowing

float V_SmithJointApprox(float NoV, float NoL, float roughness)
{
	float a = roughness * roughness;
	float visV = NoL * (NoV * (1.0 - a) + a);
	float visL = NoV * (NoL * (1.0 - a) + a);
	return 0.5 / (visV + visL);
}

float V_SmithJoint(float NoV, float NoL, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float visV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
	float visL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
	return 0.5 / (visV + visL);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    // Schlick approximation
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// Optional: fresnel with roughness to account for rough-surface desaturation
float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    // Blend between F0 and (1.0) by roughness to approximate energy shift
	return F0 + (max(1.0, F0) - F0) * pow(1.0 - cosTheta, 5.0) * roughness;
}

//

float3 GetSpecularF0(float3 albedo, float metallic)
{
	float3 dielectricF0 = float3(0.04, 0.04, 0.04); // typical default for dielectrics
	return lerp(dielectricF0, albedo, metallic); // metals use albedo as F0
}

float3 GetDiffuseReflectance(float3 albedo, float metallic)
{
	return albedo * (1.0 - metallic);
}
