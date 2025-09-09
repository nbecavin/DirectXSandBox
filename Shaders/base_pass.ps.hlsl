#include "sh_const.h"
#include "sh_math.h"
#include "material_common.hlsli"

/*Texture2D    tex0 : register( t0 );
SamplerState g_samLinear : register( s0 );*/

float4 ForwardMain(const in VS_Output i) : SV_TARGET
{
    // Camera vector from surface to camera
    //float3 V = normalize(g_CameraPos - IN.worldPos);
    float3 V = i.eyevec;

    // Sample material
    MaterialPBR mat = SampleMaterial(i);
    if (mat.opacity < 0.5f)
        discard;

    mat.roughness = 0.25; //force

	// Basic directional sun lighting
	float3 DLIGHT_DIR = normalize( float3(0,1,0.55) );
	float3 DLIGHT_COLOR = 4 * float3( 1.0, 1, 1.0 );

	float3 STATIC_AMBIENT = float3( 0.1, 0.1, 0.15 );

#if 1
	// Evaluate BRDF for sun
	float3 radiance = EvaluateBRDF(mat, DLIGHT_COLOR, DLIGHT_DIR, mat.normal, V);
	radiance += STATIC_AMBIENT * mat.albedo; //should be GI instead... or skyligh
#else
	// Diffuse term
	float NoL = saturate( dot( DLIGHT_DIR, mat.normal ) );
	float light = NoL * DLIGHT_COLOR + STATIC_AMBIENT;

	// Specular term
	float3 ref = reflect( -normalize(i.eyevec), mat.normal );
	float  EyeDotR = saturate(dot(normalize(DLIGHT_DIR),ref));

	float spec = saturate( pow(EyeDotR, 96 ) );
	//light += spec*4;

    float3 radiance = light * /*i.color **/ mat.albedo;
#endif

	//radiance = mat.normal;

	return float4(LinearToSRGB(radiance, false), mat.opacity);
}
	
float4 GbufferMain( const in VS_Output i ) : SV_TARGET
{
	float albedo_alpha = sAlbedo.Sample(sSampler, i.uv).a;
	float3 normal = sNormal.Sample(sSampler, i.uv).xyz*0.5+0.5;//i.normal;

	// get normal in world space
	return float4(normal,albedo_alpha);
}
