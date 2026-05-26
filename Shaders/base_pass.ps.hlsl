#include "sh_const.h"
#include "sh_math.h"
#include "material_common.hlsli"

#include "gbuffer.hlsli"

float4 ForwardMain(const in VS_Output i) : SV_TARGET
{
	float2 pixel_pos = i.position.xy / float2(1920, 1080);
	float3 screen_uv = float3(pixel_pos, i.position.z / i.position.w);
	
    // Camera vector from surface to camera
    float3 V = normalize(Camera.eyeWorld - i.world_position);

    // Sample material
    MaterialPBR mat = SampleMaterial(i);
    if (mat.opacity < 0.5f)
        discard;

	// Sample sun shadow buffer
	float shadow = ShadowBuffer.Sample(sSampler, screen_uv.xy).r;

	// Evaluate BRDF for sun
	float3 radiance = EvaluateBRDF(mat, DLIGHT_COLOR * shadow, DLIGHT_DIR, mat.normal, V);
	radiance += STATIC_AMBIENT * mat.albedo; //should be GI instead... or skylight

	//radiance = shadow;
	//radiance = ShadowBuffer.Sample(sSampler, screen_uv.xy).r;
	//radiance = ShadowBuffer.Sample(sSampler, i.uv.xy).rgb;
	//radiance = float3(screen_uv.xy, 0);
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
	gbuffer.shading_normal = 1;
	mat.normal;
	
	return PackedGBufferRT::Pack(gbuffer);
}
