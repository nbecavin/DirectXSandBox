#include "sh_const.h"
#include "sh_math.h"
#include "material_common.hlsli"

/*Texture2D    tex0 : register( t0 );
SamplerState g_samLinear : register( s0 );*/

float4 ForwardMain(const in VS_Output i) : SV_TARGET
{
	//StructuredBuffer<MaterialParameter> materialStore = GetBindlessResourceUniform(Object.materialID);
	
    // Camera vector from surface to camera
    float3 V = normalize(Camera.eyeWorld - i.world_position);

    // Sample material
    MaterialPBR mat = SampleMaterial(i);
    if (mat.opacity < 0.5f)
        discard;

    mat.roughness = 0.25; //force

	// Basic directional sun lighting
	float3 DLIGHT_DIR = normalize( float3(0,1,0.55) );
	float3 DLIGHT_COLOR = 4 * float3( 1.0, 1, 1.0 );

	float3 STATIC_AMBIENT = float3( 0.1, 0.1, 0.15 );

	// Evaluate BRDF for sun
	float3 radiance = EvaluateBRDF(mat, DLIGHT_COLOR, DLIGHT_DIR, mat.normal, V);
	radiance += STATIC_AMBIENT * mat.albedo; //should be GI instead... or skyligh

	//radiance = mat.normal;
	//radiance = -V;

	if(Global.Visualize == EVIZ_SHOW_NORMAL)
		return float4(mat.normal * 0.5 + 0.5, mat.opacity);
	if (Global.Visualize == EVIZ_SHOW_ROUGHNESS)
		return float4(mat.roughness.xxx, mat.opacity);
	if (Global.Visualize == EVIZ_SHOW_METAL)
		return float4(mat.metallic.xxx, mat.opacity);
	
	return float4(radiance, mat.opacity);
}

float4 GbufferMain( const in VS_Output i ) : SV_TARGET
{
	MaterialParameter p = GetMaterial(Instance.materialID);
	
	float albedo_alpha = GetTexture2D(p.albedo_map).Sample(sSampler, i.uv).a;
	float3 normal = GetTexture2D(p.normal_map).Sample(sSampler, i.uv).xyz * 0.5 + 0.5; //i.normal;

	// get normal in world space
	return float4(normal,albedo_alpha);
}
