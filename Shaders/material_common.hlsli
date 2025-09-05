#ifndef __MATERIAL_COMMON_HH__
#define __MATERIAL_COMMON_HH__

// -------------------------------
// Physically Based Materials
// -------------------------------

SamplerState sSampler : register(s0);
#define LinearSampler sSampler
Texture2D sAlbedo : register(t0);
Texture2D sNormal : register(t1);
Texture2D sSpecular : register(t2);

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

// BRDF: GGX / Smith-Schlick
// -------------------------------

// GGX / Trowbridge-Reitz NDF
float DistributionGGX(float NdotH, float roughness)
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
float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	return ggx1 * ggx2;
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

MaterialPBR SampleMaterial(in VS_Output i)
{
	MaterialPBR mat = (MaterialPBR) 0;

	float4 albedo_and_opacity = sAlbedo.Sample(LinearSampler, i.uv);

    // If your albedo texture is stored in sRGB and sampled as linear by sampler state, you can skip SRGBToLinear.
    // If you get sRGB values unconverted, convert:
    // albedo = SRGBToLinear(albedo);

	float roughness = sSpecular.Sample(LinearSampler, i.uv).rgb;
    /*float3 mra = g_MRATexture.Sample(LinearSampler, IN.uv).rgb;
    float metallic = mra.r;
    float roughness = mra.g;
    float ao = mra.b;*/

	float3 emissive = 0; //g_EmissiveTex.Sample(LinearSampler, IN.uv).rgb;

    // Normal mapping
	float3 normal = sNormal.Sample(sSampler, i.uv).xyz;// * 2 - 1;
	normal = normalize(mul(i.tbn, normal)); //tangent space to local space

	mat.albedo = albedo_and_opacity.xyz;
	mat.opacity = albedo_and_opacity.w;
	mat.normal = normal;
	mat.metallic = saturate(0); // metallic);
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
	float3 dielectricF0 = float3(0.04, 0.04, 0.04); // typical default for dielectrics
	float3 F0 = lerp(dielectricF0, m.albedo, m.metallic); // metals use albedo as F0

    // NdotV, NdotL etc.
	float NdotV = max(dot(N, Vdir), 0.0001);
	float NdotL = max(dot(N, Ldir), 0.0);
	float NdotH = max(dot(N, H), 0.0);
	float VdotH = max(dot(Vdir, H), 0.0);

    // Cook-Torrance BRDF
	float D = DistributionGGX(NdotH, m.roughness);
	float G = GeometrySmith(NdotV, NdotL, m.roughness);
	float3 F = FresnelSchlick(VdotH, F0);

	float3 numerator = D * G * F;
	float denominator = 4.0 * NdotV * NdotL + 1e-6;
	float3 specular = numerator / denominator;

    // kS is Fresnel, kD is diffuse contribution (energy conservation)
	float3 kS = F;
	float3 kD = (1.0 - kS) * (1.0 - m.metallic); // metals have no diffuse

    // Lambertian diffuse (albedo / PI)
	float3 diffuse = kD * m.albedo / PI;

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


#if 0
// -------------------------------
// PBR Utility & Material Eval
// -------------------------------

// Samplers & textures (bind/registers are placeholders, adapt to your pipeline)
SamplerState LinearSampler : register(s0);
SamplerState PointSampler : register(s1);

Texture2D    g_AlbedoTex : register(t0); // sRGB usually (engine dependent)
Texture2D    g_NormalTex : register(t1); // normal in tangent space (normal map)
Texture2D    g_MRATexture : register(t2); // metallic (R), roughness (G), AO (B) packed, or separate maps
Texture2D    g_EmissiveTex : register(t3);

Texture2D    g_BRDFLUT : register(t6); // 2D precomputed BRDF integration map (x: roughness, y: NdotV -> returns scale, bias)

// -------------------------------
// Main PBR material evaluation
// -------------------------------

// Evaluate PBR lighting given material, geometry and view
float3 EvaluatePBR(Material m, PS_IN IN, float3 V)
{
    float3 N = normalize(IN.N);
    float3 Vdir = normalize(V); // view direction (world space) pointing from surface to camera
    float3 Ldir = normalize(-g_LightDirection); // assuming directional light direction stored as "light forward"
    float3 H = normalize(Vdir + Ldir);

    // Calculate base reflectivity F0 (dielectric vs metallic)
    float3 dielectricF0 = float3(0.04, 0.04, 0.04); // typical default for dielectrics
    float3 F0 = lerp(dielectricF0, m.albedo, m.metallic); // metals use albedo as F0

    // NdotV, NdotL etc.
    float NdotV = max(dot(N, Vdir), 0.0001);
    float NdotL = max(dot(N, Ldir), 0.0);
    float NdotH = max(dot(N, H), 0.0);
    float VdotH = max(dot(Vdir, H), 0.0);

    // Cook-Torrance BRDF
    float D = DistributionGGX(NdotH, m.roughness);
    float G = GeometrySmith(NdotV, NdotL, m.roughness);
    float3 F = FresnelSchlick(VdotH, F0);

    float3 numerator = D * G * F;
    float denominator = 4.0 * NdotV * NdotL + 1e-6;
    float3 specular = numerator / denominator;

    // kS is Fresnel, kD is diffuse contribution (energy conservation)
    float3 kS = F;
    float3 kD = (1.0 - kS) * (1.0 - m.metallic); // metals have no diffuse

    // Lambertian diffuse (albedo / PI)
    float3 diffuse = kD * m.albedo / PI;

    // Direct lighting (single directional light)
    float3 radiance = g_LightColor * g_LightIntensity; // incoming radiance from light
    float3 Lo = (diffuse + specular) * radiance * NdotL;

    // Image-based lighting (IBL) for ambient/specular
    // Diffuse IBL: use irradiance map (low-frequency)
    float3 irradiance = g_EnvIrradiance.Sample(LinearSampler, /* direction -> sample method depends on representation */ N).rgb;
    float3 diffuseIBL = irradiance * m.albedo;

    // Specular IBL: sample prefiltered specular environment map + BRDF LUT
    float3 R = reflect(-Vdir, N);
    float3 prefilteredColor = PrefilteredEnvSample(R, m.roughness);
    float2 brdf = IntegrateBRDF(NdotV, m.roughness).xy; // returns scale and bias typically
    // Combine: prefiltered * (F * brdf.x + brdf.y)
    float3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    // Ambient occlusion modulate IBL
    float3 ambient = (diffuseIBL * (1.0 - kS) + specularIBL) * m.ao;

    // Final color
    float3 color = Lo + ambient + m.emission;

    // Tone mapping & exposure (simple)
    color = color * g_Exposure;
    // ACES approximated or Reinhard tone mapping could be applied here. For example:
    // color = color / (color + 1.0);

    // Gamma correction if writing to sRGB render target (often hardware does this)
    // color = pow(color, 1.0/2.2);

    return color;
}

// -------------------------------
// Pixel Shader entry (example)
// -------------------------------
float4 PSMain(PS_IN IN) : SV_Target
{
    // Camera vector from surface to camera
    float3 V = normalize(g_CameraPos - IN.worldPos);

// Sample material
Material mat = SampleMaterial(IN);

// Evaluate PBR
float3 outColor = EvaluatePBR(mat, IN, V);

// Apply simple tonemapping + gamma if needed (engine-specific)
// Example: simple Reinhard
outColor = outColor / (outColor + float3(1.0,1.0,1.0));

// sRGB conversion if writing to sRGB RT is not automatic:
// outColor = pow(outColor, 1.0 / 2.2);

return float4(outColor, 1.0);
}
#endif

#endif //__MATERIAL_COMMON_HH__
