#include "sh_const.h"
#include "sh_math.h"
#include "material_common.hlsli"

#include "gbuffer.hlsli"

/*SamplerState g_samLinear : register( s0 );*/

float4 ForwardMain(const in VS_Output i) : SV_TARGET
{
    // Camera vector from surface to camera
    float3 V = normalize(Camera.eyeWorld - i.world_position);

    // Sample material
    MaterialPBR mat = SampleMaterial(i);
    if (mat.opacity < 0.5f)
        discard;

	// Basic directional sun lighting
	float3 DLIGHT_DIR = normalize( float3(0,1,0.55) );
	float3 DLIGHT_COLOR = 4 * float3( 1.0, 1, 1.0 );

	float3 STATIC_AMBIENT = float3( 0.1, 0.1, 0.15 );
	
	// Sample shadow buffer
	float shadow = ShadowBuffer.Sample(sSampler, i.uv).r;

	// Evaluate BRDF for sun
	float3 radiance = EvaluateBRDF(mat, DLIGHT_COLOR * shadow, DLIGHT_DIR, mat.normal, V);
	radiance += STATIC_AMBIENT * mat.albedo; //should be GI instead... or skylight

	//radiance = shadow;
	//radiance = mat.normal;
	//radiance = -V;
	return float4(radiance, mat.opacity);
}

PackedGBufferRT GbufferMain(const in VS_Output i)
{	
	MaterialParameter p = GetMaterial(Instance.materialID);
	GBuffer gbuffer = (GBuffer) 0;
	
	// Sample material
	MaterialPBR mat = SampleMaterial(i);
	
	gbuffer.albedo = mat.albedo;
	gbuffer.opacity = mat.opacity;
	gbuffer.shading_normal = mat.normal;
	
	return PackedGBufferRT::Pack(gbuffer);
}
